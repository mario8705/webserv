//
// Created by Alexis Lavaud on 08/05/2023.
//

#include <sys/stat.h>
#include "MountPoint.h"
#include "Regex/Pattern.h"
#include "Http/Request.h"
#include "Http/Response.h"
#include "Http/HttpException.h"
#include "VirtualHost.h"
#include "Webserv.h"
#include "Http/CGIRequestHandler.h"
#include "Http/HttpProtocolCodec.h"
#include "string_utils.hpp"
#include "Http/HttpStatusCode.h"
#include <iostream>


MountPoint::MountPoint(VirtualHost *virtualHost, RouteMatch routeMatch, const std::string &path)
    : m_virtualHost(virtualHost), m_routeMatch(routeMatch), m_path(path)
{
    m_errorDocuments[404] = "404.html"; /* TODO inject from configuration */
}

MountPoint::~MountPoint()
{
    std::vector<MountPoint *>::iterator it;

    for (it = m_nestedMounts.begin(); it != m_nestedMounts.end(); ++it)
    {
        delete *it;
    }
}

bool MountPoint::Matches(const std::string &path) const
{
    switch (m_routeMatch)
    {
    case kRouteMatch_Exact:
        break ;

        case kRouteMatch_Regex:
            return Pattern::Matches(m_path, path);

        case kRouteMatch_StartsWith:
            return !path.compare(0, m_path.size(), m_path);

    default:
        break;
    }
    return false;
}

#include <dirent.h>
#include <stdio.h>

void MountPoint::HandleRequest(Request *request, Response *response)
{
    std::vector<MountPoint *>::iterator it;
    MountPoint *mountPoint;
    URL url = request->GetUrl();
    std::string path;
    struct stat st;

    std::map<std::string, std::string> params;
    std::map<std::string, std::string>::const_iterator params_it;

    if (request->GetMethod() == kHttpMethod_Invalid)
    {
        throw HttpException(HttpStatusCode::MethodNotAllowed);
    }

    PopulateCgiParams(request, params);

    path = LocateFile(url);
    if (stat(path.c_str(), &st) >= 0)
    {
        if (S_ISDIR(st.st_mode))
        {
            DIR *dirp;
            struct dirent *dent;

            response->AddHeader("Content-Type", "text/html");

            if (NULL != (dirp = opendir(path.c_str())))
            {
                while (NULL != (dent = readdir(dirp)))
                {
                    response->Write("<p>");
                    response->Write(dent->d_name);
                    response->Write("</p>");
                }
                closedir(dirp);
            }
            return ;
        }
        else
        {
            if (utils::ends_with(path, ".php")) {
                if (response->CgiPass(request, url.GetAbsolutePath(m_root), "/usr/bin/php-cgi"/* path */ )) //todo: replace path with the True Path in the config file
                    return;
            }
            else {
                if (response->SendFile(path, st.st_size))
                    return;
            }
        }
    }
    for (it = m_nestedMounts.begin(); it != m_nestedMounts.end(); ++it)
    {
        mountPoint = *it;

        if (mountPoint->Matches(request->GetRawPath()))
        {
            return mountPoint->HandleRequest(request, response);
        }
    }
    /* TODO Exceptions aren't working anymore */
    throw HttpException(HttpStatusCode::NotFound);
}

bool MountPoint::HandleException(Request *request, Response *response, HttpException *e)
{
    std::map<int, std::string>::const_iterator it;
    std::string path;

    it = m_errorDocuments.find(e->GetStatus().GetStatusCode());
    if (m_errorDocuments.end() != it)
    {
        path = LocateFile(URL(it->second));

        /* TODO match response status to exception's */
        if (response->SendFile(path))
        {
            return true;
        }
    }
    return false;
}

void MountPoint::AddNestedMount(MountPoint *mountPoint)
{
    m_nestedMounts.push_back(mountPoint);
}

std::string MountPoint::LocateFile(const URL &url) const
{
    if (!m_root.empty())
        return url.GetAbsolutePath(m_root);
    return url.GetAbsolutePath(m_virtualHost->GetRootMountPoint()->GetRoot());
}

void MountPoint::SetRoot(const std::string &root)
{
    m_root = root;
}

std::string MountPoint::GetRoot() const
{
    return m_root;
}

static std::string ResolveVars(const std::string &input, const std::map<std::string, std::string> &vars)
{
    std::map<std::string, std::string>::const_iterator it;
    std::string ret;
    size_t pos;
    size_t last;

    for (pos = 0; pos < input.size(); )
    {
        if ('$' == input[pos] && (pos == 0 || input[pos - 1] != '\\'))
        {
            last = ++pos;
            while (last < input.size() && (input[last] == '_' || std::islower(input[last])))
                ++last;
            it = vars.find(input.substr(pos, last - pos));
            if (it != vars.end()) {
                ret.insert(ret.end(), it->second.begin(), it->second.end());
                pos = last;
            }
            else
            {
                ret.insert(ret.end(), '$');
            }
        }
        else
        {
            last = pos + 1;
            while (last < input.size() && '$' != input[last])
                ++last;
            ret.insert(ret.end(), input.begin() + pos, input.begin() + last);
            pos = last;
        }
    }
    return ret;
}

void MountPoint::PopulateCgiParams(Request *request, std::map<std::string, std::string> &paramsOut)
{
    const std::map<std::string, std::string> &rootParams = Webserv::GetInstance()->GetRootCgiParams();
    std::map<std::string, std::string>::const_iterator it;
    std::map<std::string, std::string> vars;

    vars["server_protocol"] = request->GetProtocolVersion().ToString();
    vars["server_port"] = "8080";
    vars["webserv_version"] = "1.0";
    vars["server_name"] = "localhost";

    for (it = rootParams.begin(); it != rootParams.end(); ++it)
    {
        paramsOut[it->first] = ResolveVars(it->second, vars);
    }
    for (it = m_cgiParams.begin(); it != m_cgiParams.end(); ++it)
    {
        paramsOut[it->first] = ResolveVars(it->second, vars);
    }
}

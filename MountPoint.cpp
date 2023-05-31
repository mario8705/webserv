//
// Created by Alexis Lavaud on 08/05/2023.
//

#include "MountPoint.h"
#include <sys/stat.h>
#include <iostream>
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
#include "DirectoryListing.h"
#include "IO/DataBuffer.h"
#include <dirent.h>

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
    switch (m_routeMatch) {
        case kRouteMatch_Exact:
            break;

        case kRouteMatch_Regex:
            return Pattern::Matches(m_path, path);

        case kRouteMatch_StartsWith:
            return !path.compare(0, m_path.size(), m_path);

        default:
            break;
    }
    return false;
}

bool MountPoint::HandleRequest(Request *request, Response *response)
{
    std::vector<MountPoint *>::iterator it;
    MountPoint *mountPoint;
    URL url = request->GetUrl();
    std::string path;
    struct stat st;

    std::map<std::string, std::string> params;
    std::map<std::string, std::string>::const_iterator params_it;

    for (it = m_nestedMounts.begin(); it != m_nestedMounts.end(); ++it)
    {
        mountPoint = *it;

        if (mountPoint->GetRouteMatch() == kRouteMatch_Regex &&
            mountPoint->Matches(request->GetRawPath()))
        {
            return mountPoint->HandleRequest(request, response);
        }
    }
    for (it = m_nestedMounts.begin(); it != m_nestedMounts.end(); ++it)
    {
        mountPoint = *it;

        if (mountPoint->GetRouteMatch() != kRouteMatch_Regex &&
            mountPoint->Matches(request->GetRawPath()))
        {
            return mountPoint->HandleRequest(request, response);
        }
    }

    if (request->GetContentLength() > 8388608)
    {
        throw HttpException(HttpStatusCode::RequestEntityTooLarge);
    }

    if (request->GetMethod() == kHttpMethod_Invalid)
    {
        throw HttpException(HttpStatusCode::MethodNotAllowed);
    }

    path = LocateFile(url);
    if (stat(path.c_str(), &st) >= 0)
    {
        /* TODO nginx uses ends_with(path, "/") to trigger auto index */
        if (S_ISDIR(st.st_mode)) {
            if (m_autoIndexEnabled) {
                DIR *dirp;
                struct dirent *dent;

                response->AddHeader("Content-Type", "text/html; charset=utf-8");

                if (NULL != (dirp = opendir(path.c_str()))) {
                    DirectoryListing dir(dirp, url.m_path);
                    std::stringstream ss;

                    dir.Write(ss);

                    response->GetBodyBuffer()->PutString(ss.str());
                    closedir(dirp);
                }
                return true;
            }
            else
            {
                throw HttpException(HttpStatusCode::Forbidden);
            }
        }
        else
        {
            if (!m_cgiDelegate.empty()) {
                PopulateCgiParams(request, params);

                if (response->CgiPass(request, url.GetAbsolutePath(m_root), m_cgiDelegate))
                    return true;
            }
            else {
                if (response->SendFile(path, st.st_size))
                    return true;
            }
        }
    }
    return false;
}

RouteMatch MountPoint::GetRouteMatch() const
{
    return m_routeMatch;
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

void MountPoint::SetAutoIndex(bool enabled) {
    m_autoIndexEnabled = enabled;
}

void MountPoint::SetCGIDelegate(const std::string &cgiPath) {
    m_cgiDelegate = cgiPath;
}

void MountPoint::SetIndexList(const std::vector<std::string> &indexList) {
    m_indexList = indexList;
}

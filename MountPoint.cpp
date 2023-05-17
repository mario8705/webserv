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

MountPoint::MountPoint(VirtualHost *virtualHost, RouteMatch routeMatch, const std::string &path)
    : m_virtualHost(virtualHost), m_routeMatch(routeMatch), m_path(path)
{
    m_errorDocuments[404] = "404.html";
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

void MountPoint::HandleRequest(Request *request, Response *response)
{
    std::vector<MountPoint *>::iterator it;
    MountPoint *mountPoint;
    URL url(request->GetRawPath());
    std::string path;
    struct stat st;

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
            if (response->SendFile(path, st.st_size))
                return ;
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
    throw HttpException(404);
}

bool MountPoint::HandleException(Request *request, Response *response, HttpException *e)
{
    std::map<int, std::string>::const_iterator it;
    std::string path;

    it = m_errorDocuments.find(e->GetStatus());
    if (m_errorDocuments.end() != it)
    {
        path = LocateFile(URL(it->second));

        /* TODO let the request handler select the appropriate mime type */
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

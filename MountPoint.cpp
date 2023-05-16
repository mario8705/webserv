//
// Created by Alexis Lavaud on 08/05/2023.
//

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

void MountPoint::SetRoot(const std::string &root)
{
    m_root = root;
}

std::string MountPoint::GetRoot() const
{
    return m_root;
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

void MountPoint::HandleRequest(Request *request, Response *response)
{
    std::vector<MountPoint *>::iterator it;
    MountPoint *mountPoint;

    if (response->SendFile(LocateFile(URL(request->GetRawPath()))))
        return ;
    for (it = m_nestedMounts.begin(); it != m_nestedMounts.end(); ++it)
    {
        mountPoint = *it;

        if (mountPoint->Matches(request->GetRawPath()))
        {
            return mountPoint->HandleRequest(request, response);
        }
    }
    printf("[404] %s\n", request->GetRawPath().c_str());
    throw HttpException(404);
}

bool MountPoint::HandleException(Request *request, Response *response, HttpException *e)
{
    std::map<int, std::string>::const_iterator it;

    it = m_errorDocuments.find(e->GetStatus());
    if (m_errorDocuments.end() != it)
    {
        /* TODO match response status to exception's */
        /* TODO SendFile should return false if no file could be found */
        if ((response->SendFile(LocateFile(URL(it->second))), 1))
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

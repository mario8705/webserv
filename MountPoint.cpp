//
// Created by Alexis Lavaud on 08/05/2023.
//

#include "MountPoint.h"
#include "Regex/Pattern.h"

MountPoint::MountPoint(RouteMatch routeMatch, const std::string &path)
    : m_routeMatch(routeMatch), m_path(path)
{
}

MountPoint::~MountPoint()
{
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

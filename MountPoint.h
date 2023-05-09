//
// Created by Alexis Lavaud on 08/05/2023.
//

#ifndef WEBSERV_MOUNTPOINT_H
#define WEBSERV_MOUNTPOINT_H
#include <string>

enum RouteMatch
{
    kRouteMatch_Exact,
    kRouteMatch_StartsWith,
    kRouteMatch_Regex,
};

class MountPoint
{
public:
    MountPoint(RouteMatch routeMatch, const std::string &path);
    ~MountPoint();

    void SetRoot(const std::string &root);
    std::string GetRoot() const;

    bool Matches(const std::string &path) const;

private:
    RouteMatch m_routeMatch;
    std::string m_path;
    std::string m_root;
};


#endif //WEBSERV_MOUNTPOINT_H

//
// Created by Alexis Lavaud on 08/05/2023.
//

#ifndef WEBSERV_MOUNTPOINT_H
#define WEBSERV_MOUNTPOINT_H
#include "Http/RequestHandler.h"
#include "Http/URL.h"
#include <string>
#include <vector>
#include <map>

enum RouteMatch
{
    kRouteMatch_Exact,
    kRouteMatch_StartsWith,
    kRouteMatch_Regex,
};

class VirtualHost;
class HttpException;

class MountPoint : public IRequestHandler
{
public:
    MountPoint(VirtualHost *virtualHost, RouteMatch routeMatch, const std::string &path);
    ~MountPoint();

    void SetRoot(const std::string &root);
    std::string GetRoot() const;

    bool Matches(const std::string &path) const;

    void HandleRequest(Request *request, Response *response);
    bool HandleException(Request *request, Response *response, HttpException *e);

    void AddNestedMount(MountPoint *mountPoint);

    void PopulateCgiParams(Request *request, std::map<std::string, std::string> &paramsOut);

private:
    std::string LocateFile(const URL &url) const;

    VirtualHost *m_virtualHost;
    RouteMatch m_routeMatch;
    std::map<int, std::string> m_errorDocuments;
    std::string m_path;
    std::string m_root;
    std::vector<MountPoint *> m_nestedMounts;
    std::map<std::string, std::string> m_cgiParams;
};


#endif //WEBSERV_MOUNTPOINT_H

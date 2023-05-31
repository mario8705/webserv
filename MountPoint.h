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

    RouteMatch GetRouteMatch() const;

    void SetRoot(const std::string &root);
    std::string GetRoot() const;

    void SetAutoIndex(bool enabled);
    void SetCGIDelegate(const std::string &cgiPath);
    void SetIndexList(const std::vector<std::string> &indexList);

    bool Matches(const std::string &path) const;

    bool HandleRequest(Request *request, Response *response);
    bool HandleException(Request *request, Response *response, HttpException *e);

    bool Resolve(Request *req, MountPoint *&pMountPoint, std::string &realPath) const;

    void SetAllowedMethods(int allowedMethods);

    void SetErrorPage(int nCode, const std::string &path);

    void AddNestedMount(MountPoint *mountPoint);

    void PopulateCgiParams(Request *request, std::map<std::string, std::string> &paramsOut);

private:
    std::string LocateFile(const URL &url) const;

    bool TryFile(const std::string &realPath, const URL &u, Request *req, Response *res);
    MountPoint *GetBestCandidateRoute(const std::string &path) const;

    VirtualHost *m_virtualHost;
    RouteMatch m_routeMatch;
    int m_allowedMethods;
    std::vector<std::string> m_indexList;
    bool m_autoIndexEnabled;
    std::string m_cgiDelegate;
    std::map<int, std::string> m_errorDocuments;
    std::string m_path;
    std::string m_root;
    std::vector<MountPoint *> m_nestedMounts;
    std::map<std::string, std::string> m_cgiParams;
};


#endif //WEBSERV_MOUNTPOINT_H

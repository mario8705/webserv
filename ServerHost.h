//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_SERVERHOST_H
#define WEBSERV_SERVERHOST_H
#include "Network/ConnectionHandler.h"
#include "Http/RequestHandler.h"
#include <vector>
#include <string>
#include <map>

class IEventLoop;
class HttpClientHandler;
class VirtualHost;

class ServerHost : public IConnectionHandler, public IRequestHandler
{
public:
    typedef std::vector<HttpClientHandler *> tClientList;
    typedef std::map<std::string, VirtualHost *> tVirtualHostMap;

    ServerHost(IEventLoop *eventLoop);
    ~ServerHost();

    void AddVirtualHost(VirtualHost *virtualHost);

    void HandleConnection(int fd, struct sockaddr *addr, socklen_t addrlen);
    void DeferDeletion(HttpClientHandler *client);

    void ProcessDeferredActions();

    void HandleRequest(Request *request, Response *response);

    IEventLoop *GetEventLoop() const;

private:
    VirtualHost *MatchVirtualHost(std::string host);

    IEventLoop *m_eventLoop;
    tClientList m_clients;
    tClientList m_disconnectedClients;
    std::vector<VirtualHost *> m_virtualHosts;
    tVirtualHostMap m_virtualHostNames;
};


#endif //WEBSERV_SERVERHOST_H

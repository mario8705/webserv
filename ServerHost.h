//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_SERVERHOST_H
#define WEBSERV_SERVERHOST_H
#include "Network/ConnectionHandler.h"
#include <vector>

class IEventLoop;
class HttpClientHandler;

class ServerHost : public IConnectionHandler
{
public:
    typedef std::vector<HttpClientHandler *> tClientList;

    ServerHost(IEventLoop *eventLoop);
    ~ServerHost();

    void HandleConnection(int fd, struct sockaddr *addr, socklen_t addrlen);
    void DeferDeletion(HttpClientHandler *client);

    void ProcessDeferredActions();

    IEventLoop *GetEventLoop() const;

private:
    IEventLoop *m_eventLoop;
    tClientList m_clients;
    tClientList m_disconnectedClients;
};


#endif //WEBSERV_SERVERHOST_H

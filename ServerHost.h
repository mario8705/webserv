//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_SERVERHOST_H
#define WEBSERV_SERVERHOST_H
#include "ConnectionHandler.h"
#include <vector>

class IEventLoop;
class SocketEvent;

class ServerHost : public IConnectionHandler
{
public:
    typedef std::vector<SocketEvent *> tEventList;

    ServerHost(IEventLoop *eventLoop);
    ~ServerHost();

    void HandleConnection(int fd, struct sockaddr *addr, socklen_t addrlen);
    void Disconnect(SocketEvent *event);

    IEventLoop *GetEventLoop() const;

private:
    IEventLoop *m_eventLoop;
    tEventList m_events;
};


#endif //WEBSERV_SERVERHOST_H

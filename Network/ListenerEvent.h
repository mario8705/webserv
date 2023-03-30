//
// Created by alavaud on 3/14/23.
//

#ifndef WEBSERV_LISTENEREVENT_H
#define WEBSERV_LISTENEREVENT_H
#include "../IO/IOEventBase.h"
#include <sys/socket.h>

class IConnectionHandler;

class ListenerEvent : public IOEventBase
{
public:
    ~ListenerEvent();

    void NotifyRead();

    bool IsReadable() const;

    static ListenerEvent *
    CreateAndBind(IConnectionHandler *handler, struct sockaddr *addr, socklen_t addrlen, int backlog);

private:
    ListenerEvent(IConnectionHandler *handler, int fd);

    IConnectionHandler *m_handler;
};


#endif //WEBSERV_LISTENEREVENT_H

//
// Created by alavaud on 3/14/23.
//

#ifndef WEBSERV_LISTENEREVENT_H
#define WEBSERV_LISTENEREVENT_H
#include "../IO/IOEvent.h"
#include <sys/socket.h>
#include "NetworkAddress4.h"

class IEventLoop;
class IConnectionHandler;

class ListenerEvent : public IOEvent
{
public:
    ~ListenerEvent();

    void NotifyRead();
    void NotifyWrite();

    bool IsReadable() const;
    bool IsWritable() const;

    static ListenerEvent *
    CreateAndBind(IEventLoop *eventLoop, IConnectionHandler *handler, struct sockaddr *addr, socklen_t addrlen, int backlog);

    static ListenerEvent *
    CreateAndBind(IEventLoop *eventLoop, IConnectionHandler *handler, const NetworkAddress4 &addr, int backlog);

private:
    ListenerEvent(IEventLoop *eventLoop, IConnectionHandler *handler, int fd);

    IEventLoop *m_eventLoop;
    IConnectionHandler *m_handler;
};


#endif //WEBSERV_LISTENEREVENT_H

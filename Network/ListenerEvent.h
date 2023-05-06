//
// Created by alavaud on 3/14/23.
//

#ifndef WEBSERV_LISTENEREVENT_H
#define WEBSERV_LISTENEREVENT_H
#include "../IO/IOEvent.h"
#include <sys/socket.h>

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
    CreateAndBind(IConnectionHandler *handler, struct sockaddr *addr, socklen_t addrlen, int backlog);

private:
    ListenerEvent(IConnectionHandler *handler, int fd);

    IConnectionHandler *m_handler;
};


#endif //WEBSERV_LISTENEREVENT_H

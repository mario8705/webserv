//
// Created by alavaud on 3/14/23.
//

#include "ListenerEvent.h"
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include "ConnectionHandler.h"
#include "../IO/EventLoop.h"

ListenerEvent::ListenerEvent(IEventLoop *eventLoop, IConnectionHandler *handler, int fd)
        : IOEvent(fd), m_eventLoop(eventLoop), m_handler(handler)
{
    m_eventLoop->RegisterEvent(this);
}

ListenerEvent::~ListenerEvent()
{
    m_eventLoop->UnregisterEvent(this);
}

void ListenerEvent::NotifyRead()
{
    char addrbuf[255];
    sockaddr *addr;
    socklen_t addrlen;
    int fd;

    addr = reinterpret_cast<sockaddr *>(addrbuf);
    addrlen = sizeof(addrbuf);
    fd = accept(m_fd, addr, &addrlen);
    if (fd >= 0)
    {
        m_handler->HandleConnection(fd, addr, addrlen);
    }
}

void ListenerEvent::NotifyWrite()
{
}

int fd_set_non_blocking(int fd)
{
    int fl;

    fl = fcntl(fd, F_GETFL, 0);
    if (fl < 0)
        return -1;
    if (fcntl(fd, F_SETFL, fl | O_NONBLOCK) < 0)
        return -1;
    return 0;
}

ListenerEvent *ListenerEvent::CreateAndBind(IEventLoop *eventLoop, IConnectionHandler *handler,
                                            struct sockaddr *addr, socklen_t addrlen, int backlog)
{
    int fd;
    int en;

    fd = socket(addr->sa_family, SOCK_STREAM, 0);
    if (fd < 0)
        return NULL;
    en = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) < 0)
    {
        goto fail;
    }
    if (fd_set_non_blocking(fd) < 0)
    {
        goto fail;
    }
    if (bind(fd, addr, addrlen) < 0)
    {
        goto fail;
    }
    if (listen(fd, backlog) < 0)
    {
        goto fail;
    }
    return new ListenerEvent(eventLoop, handler, fd);

fail:
    close(fd);
    return NULL;
}

ListenerEvent *ListenerEvent::CreateAndBind(IEventLoop *eventLoop, IConnectionHandler *handler,
                                            const NetworkAddress4 &addr, int backlog)
{
    struct sockaddr_in sin;
    uint32_t inaddr;
    uint16_t port;

    inaddr = addr.GetAddress();
    port = addr.GetPort();
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(inaddr);
    sin.sin_port = htons(port);

    return CreateAndBind(eventLoop, handler, (struct sockaddr *)&sin, sizeof(sin), backlog);
}

bool ListenerEvent::IsReadable() const
{
    return true;
}

bool ListenerEvent::IsWritable() const
{
    return false;
}

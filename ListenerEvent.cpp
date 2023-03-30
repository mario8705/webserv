//
// Created by alavaud on 3/14/23.
//

#include "ListenerEvent.h"
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include "ConnectionHandler.h"

ListenerEvent::ListenerEvent(IConnectionHandler *handler, int fd)
        : IOEventBase(fd), m_handler(handler)
{
}

ListenerEvent::~ListenerEvent()
{
    ::close(m_fd);
}

void ListenerEvent::NotifyRead()
{
    struct sockaddr addr;
    socklen_t addrlen;
    int fd;

    addrlen = sizeof(addr);
    fd = accept(m_fd, &addr, &addrlen);
    if (fd >= 0)
    {
        m_handler->HandleConnection(fd, &addr, addrlen);
    }
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

ListenerEvent *ListenerEvent::CreateAndBind(IConnectionHandler *handler, struct sockaddr *addr, socklen_t addrlen,
                                            int backlog)
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
    return new ListenerEvent(handler, fd);

    fail:
    close(fd);
    return NULL;
}

bool ListenerEvent::IsReadable() const
{
    return true;
}

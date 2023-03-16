//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "ServerHost.h"
#include <unistd.h>
#include "EventLoop.h"
#include "SocketEvent.h"

int fd_set_non_blocking(int fd);

ServerHost::ServerHost(IEventLoop *eventLoop)
    : m_eventLoop(eventLoop)
{
}

ServerHost::~ServerHost()
{
}

void ServerHost::HandleConnection(int fd, struct sockaddr *addr, socklen_t addrlen)
{
    SocketEvent *event;

    if (fd_set_non_blocking(fd) < 0)
    {
        ::close(fd);
        return ;
    }

    printf("New connection :D\n");
    event = new SocketEvent(this, fd);

    m_eventLoop->RegisterEvent(event);

    m_events.push_back(event);
}

void ServerHost::Disconnect(SocketEvent *event)
{
    tEventList::iterator it;

    it = std::find(m_events.begin(), m_events.end(), event);
    if (it != m_events.end())
    {
        m_eventLoop->UnregisterEvent(event);

        m_events.erase(it);

        delete event;

        printf("Disconnected %p\n", event);
    }
}

IEventLoop *ServerHost::GetEventLoop() const
{
    return m_eventLoop;
}

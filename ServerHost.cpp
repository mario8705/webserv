//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "ServerHost.h"
#include <unistd.h>
#include "IO/EventLoop.h"
#include "Http/HttpClientHandler.h"

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
    HttpClientHandler *client;

    if (fd_set_non_blocking(fd) < 0)
    {
        ::close(fd);
        return ;
    }

    printf("New connection :D\n");
    client = new HttpClientHandler(this, fd);

    m_clients.push_back(client);
}

void ServerHost::DeferDeletion(HttpClientHandler *client) {
    tClientList::iterator it;

    it = std::find(m_clients.begin(), m_clients.end(), client);
    if (it != m_clients.end()) {
        m_clients.erase(it);
        printf("Disconnected %p\n", client);
        m_disconnectedClients.push_back(client);
    }
}

IEventLoop *ServerHost::GetEventLoop() const
{
    return m_eventLoop;
}

void ServerHost::ProcessDeferredActions()
{
    size_t i;

    for (i = 0; i < m_disconnectedClients.size(); ++i)
    {
        delete m_disconnectedClients[i];
    }
    m_disconnectedClients.clear();
}

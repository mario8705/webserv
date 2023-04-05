//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "Webserv.h"
#include "IO/SelectEventLoop.h"
#include "Network/ListenerEvent.h"
#include "ServerHost.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

Webserv::Webserv()
{
    m_eventLoop = new SelectEventLoop;
    m_running = false;
}

Webserv::~Webserv()
{
    delete m_eventLoop;
}

bool Webserv::CreateServer(NetworkAddress4 addr)
{
    struct sockaddr_in sin;
    uint16_t port;
    ListenerEvent *listener;

    port = addr.GetPort();
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = addr.GetAddress();
    sin.sin_port = htons(port);

    ServerHost *host;

    host = new ServerHost(m_eventLoop);

    m_hosts.push_back(host);
    listener = ListenerEvent::CreateAndBind(host,
                                            (struct sockaddr *) &sin,
                                            sizeof(sin),
                                            10);
    m_eventLoop->RegisterEvent(listener);
    m_listeners.push_back(listener);

    return false;
}

void Webserv::Run() {
    tHostList::iterator it;
    ServerHost *host;

    m_running = true;
    while (IsRunning()) {
        for (it = m_hosts.begin(); it != m_hosts.end(); ++it) {
            host = *it;
            host->ProcessDeferredActions();
        }
        if (m_eventLoop->LoopOnce())
            break ;
    }
    printf("Shutting down...\n"); /* TODO */
}

bool Webserv::IsRunning() const {
    return m_running;
}

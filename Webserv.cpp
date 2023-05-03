//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "Webserv.h"
#include "IO/SelectEventLoop.h"
#include "Network/ListenerEvent.h"
#include "ServerHost.h"
#include "VirtualHost.h"
#include "IO/BufferChain.h"

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
    m_running = 0;
}

Webserv::~Webserv()
{
    tVirtualHostList::const_iterator it;

    for (it = m_virtualHosts.begin(); it != m_virtualHosts.end(); ++it)
    {
        delete *it;
    }

    /* TODO free all ServerHost */

    delete m_eventLoop;

    BufferChain::ReleasePool();
}

bool Webserv::CreateServer(NetworkAddress4 addr)
{
    struct sockaddr_in sin;
    uint16_t port;
    ListenerEvent *listener;
    ServerHost *host;

    port = addr.GetPort();
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = addr.GetAddress();
    sin.sin_port = htons(port);

    host = new ServerHost(m_eventLoop);

    /* TODO read configuration file to extract a list of virtual hosts */

    VirtualHost *vhost1 = new VirtualHost(host);
    vhost1->m_serverNames.push_back("localhost");

    VirtualHost *vhost2 = new VirtualHost(host);
    vhost2->m_serverNames.push_back("127.0.0.1");

    host->AddVirtualHost(vhost1);
    host->AddVirtualHost(vhost2);

    m_virtualHosts.push_back(vhost1);
    m_virtualHosts.push_back(vhost2);

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

    m_running = 1;
    while (IsRunning()) {
        for (it = m_hosts.begin(); it != m_hosts.end(); ++it) {
            host = *it;
            host->ProcessDeferredActions();
        }
        if (m_eventLoop->LoopOnce())
            break ;
    }
    /* TODO printf("Shutting down...\n"); */
}

bool Webserv::IsRunning() const {
    return (m_running == 1);
}

void Webserv::Stop()
{
    m_running = 0;
}
//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "Webserv.h"
#include "SelectEventLoop.h"
#include "ListenerEvent.h"
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

    listener = ListenerEvent::CreateAndBind(new ServerHost(m_eventLoop),
                                            (struct sockaddr *) &sin,
                                            sizeof(sin),
                                            10);
    m_eventLoop->RegisterEvent(listener);
    m_listeners.push_back(listener);

    return false;
}

void Webserv::Run()
{
    m_eventLoop->Run();
}
//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "ServerHost.h"
#include <iostream>
#include <unistd.h>
#include "IO/DataBuffer.h"
#include "IO/EventLoop.h"
#include "Http/Request.h"
#include "Http/Response.h"
#include "Http/HttpStatusCode.h"
#include "Http/HttpClientHandler.h"
#include "VirtualHost.h"
#include "string_utils.hpp"
#include "Network/ListenerEvent.h"
#include "Webserv.h"

ServerHost::ServerHost(Webserv *webserv, NetworkAddress4 bindAddress)
    : m_webserv(webserv), m_eventLoop(webserv->GetEventLoop()), m_bindAddress(bindAddress)
{
    m_listenerEvent = NULL;
}

ServerHost::~ServerHost()
{
    delete m_listenerEvent;
}

bool ServerHost::Bind()
{
    if (m_listenerEvent)
        delete m_listenerEvent;
    m_listenerEvent = ListenerEvent::CreateAndBind(m_eventLoop, this, m_bindAddress, 10);
    if (m_listenerEvent)
        std::cout << "Listening on: *:" << m_bindAddress.GetPort() << std::endl;
    return (m_listenerEvent != NULL);
}

void ServerHost::HandleConnection(int fd, struct sockaddr *addr, socklen_t addrlen)
{
    HttpClientHandler *client;

    client = new HttpClientHandler(this, fd);
    m_clients.push_back(client);
}

void ServerHost::DeferDeletion(HttpClientHandler *client) {
    tClientList::iterator it;

    for (it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        if (*it == client)
            break ;
    }
    if (it != m_clients.end()) {
        m_clients.erase(it);
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

void ServerHost::AddVirtualHost(VirtualHost *virtualHost)
{
    VirtualHost::tServerNameList::const_iterator it;
    const VirtualHost::tServerNameList& serverNames = virtualHost->GetServerNames();

    m_virtualHosts.push_back(virtualHost);
    for (it = serverNames.begin(); it != serverNames.end(); ++it)
    {
        m_virtualHostNames.insert(std::make_pair(*it, virtualHost));
    }
}

bool ServerHost::HandleRequest(Request *request, Response *response)
{
    const Request::tHttpHeaders &headers = request->GetHeaders();
    Request::tHttpHeaders::const_iterator it;
    VirtualHost *virtualHost;
    std::string host;

    it = headers.find("Host");
    if (it != headers.end())
        host = it->second;
    virtualHost = MatchVirtualHost(host);

    if (NULL == virtualHost || !virtualHost->HandleRequest(request, response))
    {
        /* TODO throw a 404 */
        response->SetStatus(HttpStatusCode::NotFound);
      //  response->GetOutputBuffer()->PutString("<center><h1>404 Not Found :(</h1></center>");
    }
    return true;
}

VirtualHost *ServerHost::MatchVirtualHost(std::string host)
{
    tVirtualHostMap::const_iterator it;
    std::string serverName;
    size_t sep;

    if (!m_virtualHosts.empty()) {
        sep = host.find_first_of(':');
        if (std::string::npos != sep) {
            serverName = host.substr(0, sep);
        } else {
            host.swap(serverName);
        }
        utils::to_lower(serverName);

        it = m_virtualHostNames.find(serverName);
        if (it != m_virtualHostNames.end())
            return it->second;
        return m_virtualHosts.front();
    }
    return NULL;
}

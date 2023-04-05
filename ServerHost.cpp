//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "ServerHost.h"
#include <unistd.h>
#include "IO/DataBuffer.h"
#include "IO/EventLoop.h"
#include "Http/Request.h"
#include "Http/Response.h"
#include "Http/HttpStatusCode.h"
#include "Http/HttpClientHandler.h"
#include "VirtualHost.h"

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

void ServerHost::HandleRequest(Request *request, Response *response)
{
    const Request::tHttpHeaders &headers = request->GetHeaders();
    Request::tHttpHeaders::const_iterator it;
    VirtualHost *virtualHost;
    std::string host;

    it = headers.find("Host");
    if (it != headers.end())
        host = it->second;
    virtualHost = MatchVirtualHost(host);

    if (NULL != virtualHost)
    {
        virtualHost->HandleRequest(request, response);
    }
    else
    {
        /* TODO throw a 404 */
        response->SetStatus(HttpStatusCode::NotFound);
        response->GetOutputBuffer()->PutString("<center><h1>404 Not Found :(</h1></center>");
    }
}

/* TODO move to string_utils.hpp along with ltrim, rtrim and trim */
template <typename _CharT>
static inline std::basic_string<_CharT> &to_lower(std::basic_string<_CharT> &s)
{
    size_t i;
    typename std::basic_string<_CharT>::iterator it;

    for (it = s.begin(); it != s.end(); ++it)
    {
        *it = std::tolower(*it);
    }
    return s;
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
        to_lower(serverName);

        it = m_virtualHostNames.find(serverName);
        if (it != m_virtualHostNames.end())
            return it->second;
        return m_virtualHosts.front();
    }
    return NULL;
}

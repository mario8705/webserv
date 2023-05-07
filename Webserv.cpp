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
#include "Config/Token.h"
#include "Config/ConfigProperty.h"
#include <iostream>

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

bool Webserv::LoadConfig(const std::string &path)
{
    std::vector<Token *> tokens;
    std::vector<ConfigProperty *> rootBlocks;

    if (!Token::tokenization(path, tokens))
    {
        return false;
    }
    ConfigProperty::push_config(tokens, rootBlocks);

    for (ConfigProperty *block : rootBlocks)
    {
        if (block->IsBlockSection("http"))
        {
            ParseHttpBlock(block);
        }
        else
        {
            std::cerr << "Unknown block " << block->getParams()[0] << ", ignoring" << std::endl;
        }
    }
    return true;
}

void Webserv::ParseHttpBlock(ConfigProperty *httpBlock)
{
    const std::vector<ConfigProperty *> body = httpBlock->getBody();
    ConfigProperty *property;
    size_t i;

    for (i = 0; i < body.size(); ++i)
    {
        property = body[i];

        if (property->IsBlockSection("types"))
        {
            ParseTypesBlock(property);
        }
    }
}

void Webserv::ParseTypesBlock(ConfigProperty *typesBlock)
{
    const std::vector<ConfigProperty *> body = typesBlock->getBody();
    size_t i;
    size_t j;

    for (i = 0; i < body.size(); ++i)
    {
        const std::vector<std::string> &params = body[i]->getParams();

        if (params.size() < 2)
        {
            std::cerr << "Invalid mime type" << std::endl;
        }
        else
        {
            for (j = 1; j < params.size(); ++j)
            {
                if (m_mimeTypes.find(params[j]) != m_mimeTypes.end())
                    std::cerr << "Duplicate mime type entry for " << params[j] << std::endl;
                else
                    m_mimeTypes.insert(std::make_pair(params[j], params[0]));
            }
        }
    }
}

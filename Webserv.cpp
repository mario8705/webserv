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

class PropertyConsumer
{
public:
    explicit PropertyConsumer(ConfigProperty *property)
        : m_property(property)
    {
        m_blocks = property->getBody();
    }

    ~PropertyConsumer()
    {
        std::vector<ConfigProperty *>::iterator it;
        ConfigProperty *property;

        for (it = m_blocks.begin(); it != m_blocks.end(); ++it)
        {
            property = *it;
            if (property->IsBlock())
            {
                std::cerr << "Unknown block " << property->getParams()[0] << std::endl;
            }
            else
            {
                std::cerr << "Unknown property " << property->getParams()[0] << std::endl;
            }
        }
    }

    template <typename T, typename Function>
    void AcceptBlocks(const std::string &blockName, Function cb, T *thisPtr) {
        std::vector<ConfigProperty *>::iterator it;
        ConfigProperty *property;

        for (it = m_blocks.begin(); it != m_blocks.end();) {
            property = *it;
            if (property->GetName() == blockName) {
                if (!property->IsBlock()) {
                    std::cout << "Warning: expected a block but found a property when parsing "
                              << property->GetName() << std::endl;
                } else {
                    (thisPtr->*cb)(property);
                }
                it = m_blocks.erase(it);
            } else {
                ++it;
            }
        }
    }

    template <typename T, typename Function, typename Validator>
    void AcceptProperties(const std::string &blockName, Function cb, T *thisPtr, Validator validator)
    {
        std::vector<ConfigProperty *>::iterator it;
        ConfigProperty *property;

        for (it = m_blocks.begin(); it != m_blocks.end(); )
        {
            property = *it;
            if (blockName.empty() || property->GetName() == blockName)
            {
                if (property->IsBlock())
                {
                    std::cout << "Warning: expected a property but found a block when parsing "
                        << property->GetName() << std::endl;
                }
                else {
                    if (validator(property)) {
                        (thisPtr->*cb)(property);
                    }
                }
                it = m_blocks.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

private:
    ConfigProperty *m_property;
    std::vector<ConfigProperty *> m_blocks;
};

bool Webserv::LoadConfig(const std::string &path)
{
    std::vector<Token *> tokens;
    ConfigProperty *rootProperty;

    if (!Token::tokenization(path, tokens))
    {
        return false;
    }
    rootProperty = ConfigProperty::push_config(tokens);
    ParseConfig(rootProperty);
    delete rootProperty;
    return true;
}

void Webserv::ParseConfig(ConfigProperty *rootBlock)
{
    PropertyConsumer rootConsumer(rootBlock);

    rootConsumer.AcceptBlocks("http", &Webserv::ParseHttpBlock, this);
}

void Webserv::ParseHttpBlock(ConfigProperty *httpBlock)
{
    PropertyConsumer httpConsumer(httpBlock);

    httpConsumer.AcceptBlocks("types", &Webserv::ParseTypesBlock, this);
}

struct mime_validator
{
    bool operator()(ConfigProperty *property) const
    {
        if (property->getParams().size() >= 2)
            return true;
        std::cerr << "Error: Invalid mime type, (expected <mime> <extensions...>" << std::endl;
        return false;
    }
};

void Webserv::ParseTypesBlock(ConfigProperty *typesBlock)
{
    PropertyConsumer typesConsumer(typesBlock);

    typesConsumer.AcceptProperties("", &Webserv::ParseMimeType, this, mime_validator());
}

void Webserv::ParseMimeType(ConfigProperty *mime)
{
    size_t j;
    const std::vector<std::string> &params = mime->getParams();

    for (j = 1; j < params.size(); ++j)
    {
        if (m_mimeTypes.find(params[j]) != m_mimeTypes.end())
            std::cerr << "Duplicate mime type entry for " << params[j] << std::endl;
        else
            m_mimeTypes.insert(std::make_pair(params[j], params[0]));
    }
}
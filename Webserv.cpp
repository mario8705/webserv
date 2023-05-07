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
#include "MimeDatabase.h"

Webserv::Webserv()
{
    m_eventLoop = new SelectEventLoop;
    m_running = 0;
    m_mimeDatabase = new MimeDatabase("text/plain");
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
    delete m_mimeDatabase;

    BufferChain::ReleasePool();
}

bool Webserv::Bind()
{
    tVirtualHostList::const_iterator it;
    std::vector<NetworkAddress4>::const_iterator addrIt;
    VirtualHost *virtualHost;
    ServerHost *serverHost;

    for (it = m_virtualHosts.cbegin(); it != m_virtualHosts.cend(); ++it)
    {
        virtualHost = *it;
        const std::vector<NetworkAddress4> &bindAddresses = virtualHost->GetBindAddresses();

        for (addrIt = bindAddresses.cbegin(); addrIt != bindAddresses.cend(); ++addrIt)
        {
            serverHost = GetServerHostByAddr(*addrIt);
            if (!serverHost)
            {
                serverHost = new ServerHost(m_eventLoop, *addrIt);
                m_hosts.push_back(serverHost);

                if (!serverHost->Bind()) {
                    std::cout << "Could not bind" << std::endl; /* TODO more explicit message */
                    return false;
                }
            }
            serverHost->AddVirtualHost(virtualHost);
        }
    }
    return true;
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

ServerHost *Webserv::GetServerHostByAddr(const NetworkAddress4 &addr) const
{
    tHostList::const_iterator it;
    ServerHost *serverHost;

    for (it = m_hosts.cbegin(); it != m_hosts.cend(); ++it)
    {
        serverHost = *it;

        if (addr == serverHost->GetBindAddress())
        {
            return serverHost;
        }
    }
    return NULL;
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
    httpConsumer.AcceptBlocks("server", &Webserv::ParseServerBlock, this);
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

#include "Regex/Pattern.h"

struct listen_validator
{
    bool operator()(ConfigProperty *prop) const
    {
        const std::string &addr = prop->getParams()[1];

        if (Pattern::Matches("^[0-9]+$", addr) ||
            Pattern::Matches("^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$", addr) ||
            Pattern::Matches("^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+:[0-9]+$", addr))
        {
            return true;
        }
        std::cerr << "Invalid format for listen, expecting [ip][:port]" << std::endl;
        return false;
    }
};

class ServerBlockConsumer : public PropertyConsumer
{
public:
    explicit ServerBlockConsumer(ConfigProperty *serverBlock, VirtualHost *virtualHost)
        : PropertyConsumer(serverBlock), m_virtualHost(virtualHost)
    {
    }

    void ConsumeAll()
    {
        AcceptProperties("listen", &ServerBlockConsumer::ParseListenProperty,
                         this, listen_validator());
    }

private:
    void ParseListenProperty(ConfigProperty *configProperty)
    {
        /* TODO parse network address */
        m_virtualHost->AddListenAddress(NetworkAddress4(0, 8080));
    }

    VirtualHost *m_virtualHost;
};

void Webserv::ParseServerBlock(ConfigProperty *serverBlock)
{
    VirtualHost *virtualHost = new VirtualHost;
    ServerBlockConsumer serverBlockConsumer(serverBlock, virtualHost);

    serverBlockConsumer.ConsumeAll();
    m_virtualHosts.push_back(virtualHost);
}

void Webserv::ParseListenProperty(ConfigProperty *listenProp)
{
}

void Webserv::ParseMimeType(ConfigProperty *mime)
{
    size_t j;
    const std::vector<std::string> &params = mime->getParams();

    for (j = 1; j < params.size(); ++j)
    {
        if (!m_mimeDatabase->RegisterType(params[j], params[0]))
            std::cerr << "Duplicate mime type entry for " << params[j] << std::endl;
    }
}

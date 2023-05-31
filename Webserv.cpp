//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "Webserv.h"
#include "IO/SelectEventLoop.h"
#include "Network/ListenerEvent.h"
#include "ServerHost.h"
#include "VirtualHost.h"
#include "IO/BufferChain.h"
#include "MountPoint.h"

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
#include <string>
#include "MimeDatabase.h"

Webserv *Webserv::s_instance = NULL;

Webserv::Webserv()
{
    s_instance = this;
    m_eventLoop = new SelectEventLoop;
    m_running = 0;
    m_mimeDatabase = new MimeDatabase("text/plain");
}

Webserv::~Webserv()
{
    tVirtualHostList::const_iterator it;

    s_instance = NULL;
    for (it = m_virtualHosts.begin(); it != m_virtualHosts.end(); ++it)
    {
        delete *it;
    }

    /* TODO free all ServerHost */

    delete m_eventLoop;
    delete m_mimeDatabase;

    BufferChain::ReleasePool();
}

MimeDatabase *Webserv::GetMimeDatabase() const
{
    return m_mimeDatabase;
}

Webserv *Webserv::GetInstance()
{
    return s_instance;
}

bool Webserv::Bind()
{
    tVirtualHostList::const_iterator it;
    std::vector<NetworkAddress4>::const_iterator addrIt;
    VirtualHost *virtualHost;
    ServerHost *serverHost;

    for (it = m_virtualHosts.begin(); it != m_virtualHosts.end(); ++it)
    {
        virtualHost = *it;
        const std::vector<NetworkAddress4> &bindAddresses = virtualHost->GetBindAddresses();

        for (addrIt = bindAddresses.begin(); addrIt != bindAddresses.end(); ++addrIt)
        {
            serverHost = GetServerHostByAddr(*addrIt);
            if (!serverHost)
            {
                serverHost = new ServerHost(this, *addrIt);
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

    for (it = m_hosts.begin(); it != m_hosts.end(); ++it)
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

IEventLoop *Webserv::GetEventLoop() const
{
    return m_eventLoop;
}

bool Webserv::LoadConfig(const std::string &path)
{
    std::vector<Token *> tokens;
    ConfigProperty *rootProperty;

    if (!Token::tokenization(path, tokens))
    {
        return false;
    }
    rootProperty = new ConfigProperty;
    try {
        ConfigProperty::push_config(rootProperty, tokens);
        ParseConfig(rootProperty);
    }
    catch (std::exception &exception)
    {
        std::cerr << "Error:Bad configuration file (" << exception.what() << ")" << std::endl;
        delete rootProperty;
        return false;
    }
    delete rootProperty;
    return true;
}

#include "Config/PropertyConsumer.h"

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

    PropertyIterator it = httpBlock->FindAllProps("cgi_param");
    ConfigProperty *param;

    while ((param = it.Next()))
    {
        if (param->GetParamsCount() != 3)
        {
            /* TODO warning ? */
            continue ;
        }
        m_rootCgiParams[param->getParams()[1]] = param->getParams()[2];
    }
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
    /*Todo a faire valider par Alavaud.*/
    bool operator()(ConfigProperty *prop) const
    {
        const std::string &addr = prop->getParams()[1];
        if (prop->getParams().size() < 2 || !isdigit(prop->getParams()[1][0]))
        {
            std::cerr << "Fatal error : invalid port" << std::endl;
            return false;
        }
        if (std::atoi(addr.c_str()) >= 65536)
        {
            std::cerr << "The port should be beetween 0 and 65535" << std::endl;
            return false;
        }
        if (Pattern::Matches("^[0-9]+$", addr) ||
            Pattern::Matches("^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$", addr) ||
            Pattern::Matches("^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+:[0-9]+$", addr)
            )
        {
            return true;
        }
        std::cerr << "Invalid format for listen, expecting [ip][:port]" << std::endl;
        return false;
    }
};

struct root_validator
{
    bool operator()(ConfigProperty *) const
    {
        return true;
    }
};

class ServerLocationBlockConsumer : public PropertyConsumer
{
public:
    ServerLocationBlockConsumer(ConfigProperty *locationBlock, VirtualHost *virtualHost, MountPoint *parentRoute)
        : PropertyConsumer(locationBlock), m_virtualHost(virtualHost), m_parentRoute(parentRoute)
    {
        m_mountPoint = NULL;
    }

    void ConsumeAll()
    {
        const std::vector<std::string> &params = m_property->getParams();
        RouteMatch routeMatch;
        int pathIndex;

        if (params.size() < 2)
        {
            std::cerr << "Not enough parameters in location block" << std::endl;
            return ;
        }
        else if (params.size() > 4)
        {
            std::cerr << "Too many parameters in location block" << std::endl;
            return ;
        }

        routeMatch = kRouteMatch_StartsWith;
        pathIndex = 1;
        if (params.size() == 3)
        {
            if ("~" == params[1])
                routeMatch = kRouteMatch_Regex;
            else if ("=" == params[1])
                routeMatch = kRouteMatch_Exact;
            else
            {
                std::cerr << "Unknown modifier for location block : " << params[1] << std::endl;
            }
            ++pathIndex;
        }
        m_mountPoint = new MountPoint(m_virtualHost, routeMatch, params[pathIndex]);

        AcceptProperties("root", &ServerLocationBlockConsumer::ParseRootProperty, this, root_validator());

        m_parentRoute->AddNestedMount(m_mountPoint);
    }

private:
    VirtualHost *m_virtualHost;
    MountPoint *m_mountPoint;
    MountPoint *m_parentRoute;

    void ParseRootProperty(ConfigProperty *rootProp)
    {
        m_mountPoint->SetRoot(rootProp->getParams()[1]);
    }
};

class ServerBlockConsumer : public PropertyConsumer
{
public:
    ServerBlockConsumer(ConfigProperty *serverBlock, VirtualHost *virtualHost)
        : PropertyConsumer(serverBlock), m_virtualHost(virtualHost)
    {
    }

    void ConsumeAll()
    {
        AcceptProperties("listen", &ServerBlockConsumer::ParseListenProperty,
                         this, listen_validator());
        AcceptProperties("root", &ServerBlockConsumer::ParseRootProperty,
                     this, root_validator());
        AcceptBlocks("location", &ServerBlockConsumer::ParseLocationBlock,
                     this);
    }

private:
    void ParseListenProperty(ConfigProperty *configProperty)
    {
        /* TODO Alavaud must check is this is ok */
        int port = std::atoi(configProperty->getParams()[1].c_str());
        m_virtualHost->AddListenAddress(NetworkAddress4(0, port));
    }

    void ParseRootProperty(ConfigProperty *configProperty)
    {
        m_virtualHost->GetRootMountPoint()->SetRoot(configProperty->getParams()[1]);
    }

    void ParseLocationBlock(ConfigProperty *locationBlock)
    {
        ServerLocationBlockConsumer locationConsumer(locationBlock, m_virtualHost, m_virtualHost->GetRootMountPoint());

        locationConsumer.ConsumeAll();
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

const std::map<std::string, std::string> &Webserv::GetRootCgiParams() const
{
    return m_rootCgiParams;
}

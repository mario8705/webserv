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

void Webserv::ParseConfig(ConfigProperty *rootBlock)
{
    PropertyIterator blocksIterator = rootBlock->FindAllBlocks();
    PropertyIterator propsIterator = rootBlock->FindAllProps();
    ConfigProperty *prop;

    bool isFileRunnable = false;
    while ((prop = blocksIterator.Next()) != NULL) {
        if (prop->GetName() == "types")
        {
            ParseMimeDatabase(prop);
        }
        else if (prop->GetName() == "server")
        {
            ParseServerBlock(prop);
            isFileRunnable = true;
        }
        else
            throw std::runtime_error("Unknown property block: " + prop->GetName());
    }

    int nbValidParams = 0;
    while ((prop = propsIterator.Next()) != NULL)
    {
        if (prop->GetName() == "default_type")
        {
            if (prop->getParams().size() < 2)
            {
                std::cerr << "Warning: default_type required at least 1 parameter" << std::endl;
            }
            else
            {
                m_mimeDatabase->SetDefaultType(prop->getParams()[1]);
                ++nbValidParams;
            }
        }
        else
        {
            std::cerr << "Unknown property " << prop->GetName() << std::endl;
        }

    }

    if (!isFileRunnable || nbValidParams == 0)
        throw std::runtime_error("Empty config file");
}

void Webserv::ParseMimeDatabase(ConfigProperty *typesBlock)
{
    PropertyIterator it = typesBlock->FindAllProps();
    ConfigProperty *typeProp;
    size_t i;

    while ((typeProp = it.Next()) != NULL)
    {
        const std::vector<std::string> &params = typeProp->getParams();

        if (params.size() < 2)
        {
            throw std::runtime_error("MIME type requires at least 2 params");
        }
        for (i = 1; i < params.size(); ++i)
        {
            if (!m_mimeDatabase->RegisterType(params[i], params[0]))
            {
                std::cerr << "Warning: Duplicate MIME type for ext " << params[i] << " and type " << params[0] << std::endl;
            }
        }
    }
}

void Webserv::ParseServerBlock(ConfigProperty *serverBlock)
{
    PropertyIterator propsIterator = serverBlock->FindAllProps();
    PropertyIterator blocksIterator = serverBlock->FindAllBlocks();
    ConfigProperty *prop;
    size_t i, j;

    bool hasListenDirective = false;
    bool hasServerNameDirective = false;

    VirtualHost *virtualHost = new VirtualHost;
    m_virtualHosts.push_back(virtualHost);

    while ((prop = propsIterator.Next()) != NULL)
    {
        const std::vector<std::string> &params = prop->getParams();

        /* TODO move this in MountPoint (location block) */
        if (prop->GetName() == "index")
        {
            if (prop->getParams().size() < 2)
            {
                std::cerr << "index property requires at least 1 parameters" << std::endl;
            }
            else
            {

            }
        }
        else if (prop->GetName() == "listen")
        {
            for (i = 1; i < params.size(); ++i) {
                std::string port = prop->getParams()[i];
                int portNum;

                for (j = 0; j < port.size(); ++j) {
                    if (!std::isdigit(port[j]) || j > 5) {
                        throw std::runtime_error("Error in listen directive: Port format is invalid");
                    }
                }
                portNum = std::atoi(port.c_str());
                if (portNum > 65535) {
                    throw std::runtime_error("Error in listen directive: Port format must be in range [0-65535]");
                }
                virtualHost->AddListenAddress(NetworkAddress4((uint16_t) portNum));
                hasListenDirective = true;
            }
            if (i < 2) {
                std::cerr << "Warning: listen property requires at least 1 parameter" << std::endl;
            }
        }
        else if (prop->GetName() == "server_name")
        {
            if (params.size() < 2)
            {
                throw std::runtime_error("server_name directive requires at least 1 parameter");
            }
            else
            {
                for (i = 1; i < params.size(); ++i)
                {
                    virtualHost->AddServerName(params[i]);
                }
                hasServerNameDirective = true;
            }
        }
        else
        {
            std::cerr << "Warning: Unknown property in server block " << prop->GetName() << std::endl;
        }
    }

    while ((prop = blocksIterator.Next()) != NULL)
    {
        if (prop->GetName() == "location")
        {
            ParseLocationBlock(prop, virtualHost);
        }
        else
        {
            std::cerr << "Unknown block " << prop->GetName() << std::endl;
        }
    }
    if (!hasListenDirective && !hasServerNameDirective)
        throw std::runtime_error("Empty server block configuration");
}

const std::map<std::string, std::string> &Webserv::GetRootCgiParams() const
{
    return m_rootCgiParams;
}

void Webserv::ParseLocationBlock(ConfigProperty *locationBlock, VirtualHost *virtualHost)
{
    MountPoint *mountPoint;
    RouteMatch routeMatch;
    std::string path;
    std::string root;
    std::vector<std::string> indexList;
    std::string cgiPass;
    bool autoIndex = false;
    PropertyIterator it = locationBlock->FindAllProps();
    ConfigProperty *prop;
    size_t i;

    const std::vector<std::string> &locationParams = locationBlock->getParams();

    if (locationParams.size() < 2)
    {
        throw std::runtime_error("Location block requires at least one parameters");
    }

    if (locationParams.size() > 2 && locationParams[1] != "~")
    {
        throw std::runtime_error("Unknown modifier in route name");
    }

    if (locationParams[1] == "~" && locationParams.size() == 2)
    {
        throw std::runtime_error("Route name missing in location block");
    }

    if (locationParams[1] == "~")
    {
        routeMatch = kRouteMatch_Regex;
        path = locationParams[2];
    }
    else
    {
        routeMatch = kRouteMatch_StartsWith;
        path = locationParams[1];
    }

    while ((prop = it.Next()) != NULL)
    {
        const std::vector<std::string> &params = prop->getParams();

        if (prop->GetName() == "root")
        {
            if (params.size() != 2)
            {
                std::cerr << "root property requires exactly 1 parameter" << std::endl;
            }
            else
            {
                root = params[1];
            }
        }
        else if (prop->GetName() == "index")
        {
            if (params.size() < 2)
            {
                std::cerr << "index property requires at least 1 parameter" << std::endl;
            }
            else
            {
                for (i = 1; i < params.size(); ++i)
                {
                    indexList.push_back(params[i]);
                }
            }
        }
        else if (prop->GetName() == "autoindex")
        {
            if (params.size() != 2)
            {
                std::cerr << "autoindex property requires exactly 1 parameter" << std::endl;
            }
            else if (params[1] != "on" && params[1] != "off")
            {
                std::cerr << "parameter of autoindex must be either on or off" << std::endl;
            }
            else
            {
                autoIndex = (params[1] == "on");
            }
        }
        else if (prop->GetName() == "cgi_pass")
        {
            if (params.size() != 2)
            {
                std::cerr << "cgi_pass requires exactly one parameter" << std::endl;
            }
            else
            {
                cgiPass = params[1];
            }
        }
        else
        {
            std::cerr << "Warning: unknown property in location block " << prop->GetName() << std::endl;
        }
    }

    if (root.empty() && virtualHost->GetRootMountPoint()->GetRoot().empty())
    {
        throw std::runtime_error("Empty root in route");
    }

    mountPoint = new MountPoint(virtualHost, routeMatch, path);
    mountPoint->SetAutoIndex(autoIndex);
    mountPoint->SetRoot(root);
    mountPoint->SetCGIDelegate(cgiPass);
    mountPoint->SetIndexList(indexList);
    virtualHost->GetRootMountPoint()->AddNestedMount(mountPoint);
}

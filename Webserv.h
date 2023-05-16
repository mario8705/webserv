//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_WEBSERV_H
#define WEBSERV_WEBSERV_H
#include <csignal>
#include <string>
#include <vector>
#include <map>
#include "Network/NetworkAddress4.h"

class IEventLoop;
class ListenerEvent;
class ServerHost;
class VirtualHost;
class ConfigProperty;
class MimeDatabase;

class Webserv
{
public:
    typedef std::vector<ServerHost *> tHostList;
    typedef std::vector<VirtualHost *> tVirtualHostList;

    Webserv();
    ~Webserv();

    bool LoadConfig(const std::string &path);

    bool Bind();
    void Run();

    bool IsRunning() const;
    void Stop();

    MimeDatabase *GetMimeDatabase() const;
    IEventLoop *GetEventLoop() const;

    static Webserv *GetInstance();

private:
    IEventLoop *m_eventLoop;
    std::vector<ListenerEvent *> m_listeners;
    tHostList m_hosts;
    tVirtualHostList m_virtualHosts;
    volatile sig_atomic_t m_running;
    MimeDatabase *m_mimeDatabase;
    std::string m_defaultMimeType;

    static Webserv *s_instance;

    ServerHost *GetServerHostByAddr(const NetworkAddress4 &addr) const;

    void ParseConfig(ConfigProperty *rootBlock);
    void ParseHttpBlock(ConfigProperty *httpBlock);
    void ParseTypesBlock(ConfigProperty *typesBlock);

    void ParseServerBlock(ConfigProperty *serverBlock);
    void ParseListenProperty(ConfigProperty *listenProp);

    void ParseMimeType(ConfigProperty *mime);
};


#endif //WEBSERV_WEBSERV_H

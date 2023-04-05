//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_WEBSERV_H
#define WEBSERV_WEBSERV_H
#include <vector>
#include <map>
#include "Network/NetworkAddress4.h"

class IEventLoop;
class ListenerEvent;
class ServerHost;
class VirtualHost;

class Webserv
{
public:
    typedef std::vector<ServerHost *> tHostList;
    typedef std::vector<VirtualHost *> tVirtualHostList;

    Webserv();
    ~Webserv();

    bool CreateServer(NetworkAddress4 addr);

    void Run();

    bool IsRunning() const;

private:
    IEventLoop *m_eventLoop;
    std::vector<ListenerEvent *> m_listeners;
    tHostList m_hosts;
    tVirtualHostList m_virtualHosts;
//    std::map<NetworkAddress4, ServerHost *> m_hosts;
    volatile bool m_running;
};


#endif //WEBSERV_WEBSERV_H

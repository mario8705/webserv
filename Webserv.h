//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_WEBSERV_H
#define WEBSERV_WEBSERV_H
#include <vector>
#include <map>
#include "NetworkAddress4.h"

class IEventLoop;
class ListenerEvent;
class ServerHost;

class Webserv
{
public:
    Webserv();
    ~Webserv();

    bool CreateServer(NetworkAddress4 addr);

    void Run();

private:
    IEventLoop *m_eventLoop;
    std::vector<ListenerEvent *> m_listeners;
    std::map<NetworkAddress4, ServerHost *> m_hosts;
};


#endif //WEBSERV_WEBSERV_H

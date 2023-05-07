//
// Created by Alexis Lavaud on 05/04/2023.
//

#ifndef WEBSERV_VIRTUALHOST_H
#define WEBSERV_VIRTUALHOST_H
#include "Http/RequestHandler.h"
#include <string>
#include <vector>
#include "Network/NetworkAddress4.h"

class ServerHost;

class VirtualHost : public IRequestHandler
{
public:
    typedef std::vector<std::string> tServerNameList;

    VirtualHost();
    ~VirtualHost();

    void AddListenAddress(const NetworkAddress4 &addr);

    const std::vector<NetworkAddress4> &GetBindAddresses() const
    {
        return m_bindAddresses;
    }

    void HandleRequest(Request *request, Response *response);

    const tServerNameList &GetServerNames() const;

    tServerNameList m_serverNames;
    std::vector<NetworkAddress4> m_bindAddresses;
};

#endif //WEBSERV_VIRTUALHOST_H

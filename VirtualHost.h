//
// Created by Alexis Lavaud on 05/04/2023.
//

#ifndef WEBSERV_VIRTUALHOST_H
#define WEBSERV_VIRTUALHOST_H
#include "Http/RequestHandler.h"
#include <string>
#include <vector>

class VirtualHost : public IRequestHandler
{
public:
    typedef std::vector<std::string> tServerNameList;

    VirtualHost();
    ~VirtualHost();

    void HandleRequest(Request *request, Response *response);

    const tServerNameList &GetServerNames() const;

    tServerNameList m_serverNames;
};

#endif //WEBSERV_VIRTUALHOST_H

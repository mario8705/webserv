//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_REQUEST_H
#define WEBSERV_REQUEST_H
#include <string>
#include "HttpMethod.h"

class Request
{
public:
    virtual ~Request();

    HttpMethod GetMethod() const;
    std::string GetRawPath() const;

protected:
    Request();

    HttpMethod m_method;
    std::string m_rawPath;
};


#endif //WEBSERV_REQUEST_H

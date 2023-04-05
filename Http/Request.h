//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_REQUEST_H
#define WEBSERV_REQUEST_H
#include <string>
#include <map>
#include "HttpMethod.h"
#include "HttpVersion.h"

class Request
{
public:
    typedef std::map<std::string, std::string> tHttpHeaders;

    virtual ~Request();

    HttpMethod GetMethod() const;
    std::string GetRawPath() const;

    const tHttpHeaders &GetHeaders() const;

protected:
    explicit Request(tHttpHeaders &headers);

    HttpMethod m_method;
    std::string m_rawPath;
    tHttpHeaders &m_headers;
};


#endif //WEBSERV_REQUEST_H

//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_REQUEST_H
#define WEBSERV_REQUEST_H
#include <string>
#include <map>
#include "HttpMethod.h"
#include "HttpVersion.h"

class HttpProtocolCodec;
class HttpClientHandler;

class Request
{
public:
    typedef std::map<std::string, std::string> tHttpHeaders;

    virtual ~Request();

    HttpMethod GetMethod() const;
    std::string GetRawPath() const;

    const tHttpHeaders &GetHeaders() const;

    HttpProtocolCodec *GetHttpCodec() const;

protected:
    explicit Request(tHttpHeaders &headers);

    HttpClientHandler *m_clientHandler;
    HttpMethod m_method;
    std::string m_rawPath;
    tHttpHeaders &m_headers;
    HttpProtocolCodec *m_codec;
};


#endif //WEBSERV_REQUEST_H

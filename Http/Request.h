//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_REQUEST_H
#define WEBSERV_REQUEST_H
#include <string>
#include <map>
#include "HttpMethod.h"
#include "HttpVersion.h"
#include "URL.h"

class HttpProtocolCodec;
class HttpClientHandler;

class Request
{
    friend class HttpProtocolCodec;

public:
    typedef std::map<std::string, std::string> tHttpHeaders;

    virtual ~Request();

    HttpMethod GetMethod() const;
    std::string GetRawPath() const;

    URL GetUrl() const;

    HttpVersion GetProtocolVersion() const;

    const tHttpHeaders &GetHeaders() const;

    HttpProtocolCodec *GetHttpCodec() const;

    size_t GetContentLength() const;
    std::string GetContentType() const;

    void EncodeCGIHeaders(std::map<std::string, std::string> &cgiEnv);

protected:
    Request(HttpClientHandler *clientHandler, tHttpHeaders &headers);

    void SetMethod(HttpMethod method);
    void SetRawPath(const std::string &rawPath);
    void SetProtocolVersion(const HttpVersion& httpVersion);

private:
    HttpClientHandler *m_clientHandler;
    URL m_url;
    HttpMethod m_method;
    HttpVersion m_httpVersion;
    std::string m_rawPath;
    tHttpHeaders &m_headers;
    HttpProtocolCodec *m_codec;
};


#endif //WEBSERV_REQUEST_H

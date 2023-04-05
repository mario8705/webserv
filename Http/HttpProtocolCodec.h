//
// Created by Alexis Lavaud on 23/03/2023.
//

#ifndef WEBSERV_HTTPPROTOCOLCODEC_H
#define WEBSERV_HTTPPROTOCOLCODEC_H
#include <string>
#include <map>
#include "HttpMethod.h"
#include "HttpVersion.h"

class HttpClientHandler;
class DataBuffer;
class Response;

class HttpProtocolCodec
{
public:
    typedef std::map<std::string, HttpMethod> tHttpMethodsMap;
    typedef std::map<std::string, HttpVersion> tHttpVersionsMap;
    typedef std::map<std::string, std::string> tHeaderMap;

    explicit HttpProtocolCodec(HttpClientHandler *handler);
    ~HttpProtocolCodec();

    void ProcessDataInput(DataBuffer *in);

    void EncodeResponse(DataBuffer *out, Response *response);
    void FinalizeResponse();

private:
    void ParseRequestHeader(const std::string &line);
    void SetRequestHeader(const std::string &method, const std::string &rawPath, const std::string &httpVersion);
    void ParseHeader(const std::string &line);
    void WriteResponseHeader(DataBuffer *out, int status, const std::string &statusMessage);
    void WriteHeaders(DataBuffer *out, const tHeaderMap &headers);

    void DispatchRequest();

    HttpClientHandler *m_handler;
    tHttpMethodsMap m_methods;
    tHttpVersionsMap m_httpVersions;
    tHeaderMap m_headers;
    bool m_requestHeaderParsed;
    HttpMethod m_method;
    HttpVersion m_httpVersion;
    std::string m_rawPath;
};


#endif //WEBSERV_HTTPPROTOCOLCODEC_H

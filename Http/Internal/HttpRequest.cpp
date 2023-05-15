//
// Created by Alexis Lavaud on 04/04/2023.
//

#include "HttpRequest.h"
#include "../HttpClientHandler.h"

HttpRequest::HttpRequest(HttpClientHandler *clientHandler, HttpMethod method, const std::string &rawPath, HttpVersion version,
                         Request::tHttpHeaders &headers)
                         : Request(headers)
{
    m_clientHandler = clientHandler;
    m_codec = clientHandler->GetProtocolCodec();
    m_method = method;
    m_rawPath = rawPath;
    /* m_httpVersion = version; */ /* TODO */
}

HttpRequest::~HttpRequest()
{
}


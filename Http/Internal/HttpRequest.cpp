//
// Created by Alexis Lavaud on 04/04/2023.
//

#include "HttpRequest.h"

HttpRequest::HttpRequest(HttpProtocolCodec *codec, HttpMethod method, const std::string &rawPath, HttpVersion version,
                         Request::tHttpHeaders &headers)
                         : Request(headers)
{
    m_codec = codec;
    m_method = method;
    m_rawPath = rawPath;
    /* m_httpVersion = version; */ /* TODO */
}

HttpRequest::~HttpRequest()
{
}


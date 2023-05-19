//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "Request.h"
#include "HttpClientHandler.h"

Request::Request(HttpClientHandler *clientHandler, tHttpHeaders &headers)
    : m_clientHandler(clientHandler), m_headers(headers)
{
    m_codec = m_clientHandler->GetProtocolCodec();
}

Request::~Request()
{
}

HttpMethod Request::GetMethod() const
{
    return m_method;
}

std::string Request::GetRawPath() const
{
    return m_rawPath;
}

const Request::tHttpHeaders &Request::GetHeaders() const
{
    return m_headers;
}

HttpProtocolCodec *Request::GetHttpCodec() const
{
    return m_codec;
}

HttpVersion Request::GetProtocolVersion() const
{
    return m_httpVersion;
}

void Request::SetMethod(HttpMethod method)
{
    m_method = method;
}

void Request::SetRawPath(const std::string &rawPath)
{
    m_rawPath = rawPath;
}

void Request::SetProtocolVersion(const HttpVersion& httpVersion)
{
    m_httpVersion = httpVersion;
}

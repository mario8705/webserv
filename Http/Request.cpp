//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "Request.h"

Request::Request(tHttpHeaders &headers)
    : m_headers(headers)
{
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

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

URL Request::GetUrl() const
{
    tHttpHeaders::const_iterator it;
    std::string host;
    std::string path;
    std::string query;
    size_t sep;

    if (std::string::npos == (sep = m_rawPath.find_first_of('?')))
    {
        path = m_rawPath;
    }
    else
    {
        path = m_rawPath.substr(0, sep);
        query = m_rawPath.substr(sep + 1);
    }

    it = m_headers.find("Host");
    if (m_headers.end() != it)
        host = it->second;
    return URL("http:", host, path, query);
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

static std::string GetHttpMethodString(HttpMethod m)
{
    switch (m)
    {
        case kHttpMethod_Get:
            return "GET";

        case kHttpMethod_Post:
            return "POST";

        case kHttpMethod_Patch:
            return "PATCH";

        case kHttpMethod_Delete:
            return "DELETE";

        case kHttpMethod_Head:
            return "HEAD";

        case kHttpMethod_Put:
            return "PUT";

        case kHttpMethod_Options:
            return "OPTIONS";

        default:
            return "UNKNOWN";
    }
}

void Request::EncodeCGIHeaders(std::map<std::string, std::string> &cgiEnv)
{
    tHttpHeaders::const_iterator it;
    size_t i;

    cgiEnv["REQUEST_METHOD"] = GetHttpMethodString(m_method); /* TODO move */

    /**
     * TODO security breach?? only allow headers from a predefined list?
     */
    for (it = m_headers.begin(); it != m_headers.end(); ++it)
    {
        std::string key = it->first;

        if (key == "Content-Length")
            cgiEnv["CONTENT_LENGTH"] = it->second;
        else if (key == "Content-Type")
            cgiEnv["CONTENT_TYPE"] = it->second;

        for (i = 0; i < key.size(); ++i)
        {
            if (key[i] == '-')
                key[i] = '_';
            key[i] = std::toupper(key[i]);
        }
        key.insert(0, "HTTP_");
        cgiEnv[key] = it->second;
    }
}

size_t Request::GetContentLength() const {
    tHttpHeaders::const_iterator it;

    it = m_headers.find("Content-Length");
    if (it == m_headers.end())
        return 0;
    return std::atoi(it->second.c_str());
}

std::string Request::GetContentType() const
{
    tHttpHeaders::const_iterator it;

    it = m_headers.find("Content-Type");
    if (it == m_headers.end())
        return "text/plain";
    return it->second;
}
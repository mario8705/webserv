//
// Created by Alexis Lavaud on 23/03/2023.
//

#include "HttpProtocolCodec.h"
#include <sstream>
#include "HttpClientHandler.h"
#include "../IO/DataBuffer.h"
#include "Internal/HttpRequest.h"
#include "Internal/HttpResponse.h"

inline std::string &ltrim(std::string &s)
{
    std::string::iterator it;

    for (it = s.begin(); it != s.end(); ++it)
    {
        if (!std::isspace(*it))
        {
            break ;
        }
    }
    s.erase(s.begin(), it);
    return s;
}

inline std::string &rtrim(std::string &s)
{
    std::string::reverse_iterator it;

    for (it = s.rbegin(); it != s.rend(); ++it)
    {
        if (!std::isspace(*it))
        {
            break ;
        }
    }
    s.erase(it.base(), s.end());
    return s;
}

inline std::string &trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
    return s;
}

HttpProtocolCodec::HttpProtocolCodec(HttpClientHandler *handler)
    : m_handler(handler)
{
    m_requestHeaderParsed = false;

    m_methods.insert(std::make_pair("GET", kHttpMethod_Get));
    m_methods.insert(std::make_pair("POST", kHttpMethod_Post));
    m_methods.insert(std::make_pair("PUT", kHttpMethod_Put));
    m_methods.insert(std::make_pair("PATCH", kHttpMethod_Patch));
    m_methods.insert(std::make_pair("DELETE", kHttpMethod_Delete));
    m_methods.insert(std::make_pair("HEAD", kHttpMethod_Head));
    m_methods.insert(std::make_pair("OPTIONS", kHttpMethod_Options));

    m_httpVersions.insert(std::make_pair("HTTP/1.0", kHttpVersion_1_0));
    m_httpVersions.insert(std::make_pair("HTTP/1.1", kHttpVersion_1_1));
}

HttpProtocolCodec::~HttpProtocolCodec()
{
}

void HttpProtocolCodec::ProcessDataInput(DataBuffer *in)
{
    std::string line;

    while (in->Readln(line))
    {
        if (!m_requestHeaderParsed)
        {
            ParseRequestHeader(line);
            m_requestHeaderParsed = true;

            if (kHttpVersion_1_0 == m_httpVersion)
            {
                DispatchRequest();
            }
        }
        else
        {
            if (line.empty())
            {
                DispatchRequest();
                return ;
            }
            else
            {
                ParseHeader(line);
            }
        }
    }
    if (in->GetLength() >= in->GetReadHighWatermark())
    {
        /* TODO handle veryyyy looong requests */
       // printf("Buffer full and no line could be read\n");
        //m_event->GetOutputBuffer()->PutString("Buffer full");
       // Disconnect(true);
    }
}

void HttpProtocolCodec::EncodeResponse(DataBuffer *out, Response *response)
{
    WriteResponseHeader(out, response->GetStatusCode(), response->GetStatusMessage());
    WriteHeaders(out, response->GetHeaders());
    out->PutString("\r\n");

    out->AddBuffer(response->GetOutputBuffer());
}

void HttpProtocolCodec::ParseRequestHeader(const std::string &line)
{
    size_t methodSep;
    size_t httpVersionSep;
    std::string method;
    std::string httpVersion;
    std::string rawPath;

    methodSep = line.find_first_of(' ');
    httpVersionSep = line.find_last_of(' ');
    httpVersion = "HTTP/1.0";
    if (methodSep == std::string::npos)
    {
        printf("Invalid request\n");
        return ;
    }
    method = line.substr(0, methodSep);
    if (httpVersionSep != std::string::npos && httpVersionSep != methodSep)
    {
        httpVersion = line.substr(httpVersionSep + 1);
        rawPath = line.substr(methodSep, line.size() - methodSep - httpVersion.size());
    }
    else
    {
        rawPath = line.substr(methodSep + 1);
    }
    SetRequestHeader(method, rawPath, httpVersion);
}

void HttpProtocolCodec::SetRequestHeader(const std::string &method, const std::string &rawPath,
                                         const std::string &httpVersion)
{
    tHttpVersionsMap::const_iterator versionsIt = m_httpVersions.find(httpVersion);
    if (versionsIt == m_httpVersions.end())
    {
        printf("Invalid request : Unknown http version\n");
        return ;
    }

    tHttpMethodsMap::const_iterator methodsIt = m_methods.find(method);
    if (methodsIt == m_methods.end())
    {
        printf("Invalid request : Unknown http method\n");
        return ;
    }

    m_method = methodsIt->second;
    m_httpVersion = versionsIt->second;
    m_rawPath = rawPath;
    trim(m_rawPath);
}

void HttpProtocolCodec::ParseHeader(const std::string &line)
{
    size_t sep;
    std::string key;
    std::string value;

    sep = line.find_first_of(':');
    if (sep != std::string::npos)
    {
        key = line.substr(0, sep);
        value = line.substr(sep + 1);
    }
    else
    {
        key = line;
    }
    trim(key);
    trim(value);
    m_headers.insert(std::make_pair(key, value));
}

void HttpProtocolCodec::WriteResponseHeader(DataBuffer *out, int status, const std::string &statusMessage)
{
    tHttpVersionsMap::const_iterator it;
    std::string version;
    std::stringstream ss;

    for (it = m_httpVersions.begin(); it != m_httpVersions.end(); ++it)
    {
        if (it->second == m_httpVersion)
        {
            version = it->first;
            break ;
        }
    }
    if (it == m_httpVersions.end())
    {
        /* TODO Should never happend */
    }
    ss << version << " " << status << " " << statusMessage << "\r\n";
    out->PutString(ss.str());
}

void HttpProtocolCodec::WriteHeaders(DataBuffer *out, const tHeaderMap &headers)
{
    tHeaderMap::const_iterator it;
    std::stringstream ss;

    for (it = headers.begin(); it != headers.end(); ++it)
    {
        ss << it->first << ": " << it->second << "\r\n";
    }
    out->PutString(ss.str());
}

void HttpProtocolCodec::DispatchRequest()
{
    HttpRequest request(m_method, m_rawPath, m_httpVersion, m_headers);
    HttpResponse response;

    m_handler->HandleRequest(&request, &response);

}

void HttpProtocolCodec::FinalizeResponse()
{
    tHeaderMap::const_iterator it;

    if (kHttpVersion_1_0 == m_httpVersion)
    {
        m_handler->Disconnect(true);
        return ;
    }
    else if (kHttpVersion_1_1 == m_httpVersion)
    {
        it = m_headers.find("Connection");

        if (it != m_headers.end() && it->second == "close")
        {
            m_handler->Disconnect(true);
            return ;
        }
    }

    m_headers.clear();
    m_requestHeaderParsed = false;
}

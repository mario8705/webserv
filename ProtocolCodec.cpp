//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "ProtocolCodec.h"
#include <sstream>
#include <stdexcept>
#include "DataBuffer.h"
#include "HttpException.h"
#include "RequestHandler.h"

ProtocolCodec::ProtocolCodec(IRequestHandler *requestHandler, DataBuffer *inputBuffer, DataBuffer *outputBuffer)
    : m_requestHandler(requestHandler), m_inputBuffer(inputBuffer), m_outputBuffer(outputBuffer)
{
    m_lineNumber = 0;

    m_supportedMethods.insert(std::make_pair("GET", kHttpMethod_Get));
    m_supportedMethods.insert(std::make_pair("POST", kHttpMethod_Post));
    m_supportedMethods.insert(std::make_pair("PUT", kHttpMethod_Put));
    m_supportedMethods.insert(std::make_pair("PATCH", kHttpMethod_Patch));
    m_supportedMethods.insert(std::make_pair("DELETE", kHttpMethod_Delete));
    m_supportedMethods.insert(std::make_pair("HEAD", kHttpMethod_Head));
    m_supportedMethods.insert(std::make_pair("OPTIONS", kHttpMethod_Options));
}

void ProtocolCodec::ProcessData()
{
    std::string line;

    try {
        while (m_inputBuffer->Readln(line)) {
            if (m_lineNumber == 0) {
                ParseRequest(line);
            } else {
                if (line.empty()) {
                    DispatchRequest();

                    m_lineNumber = 0;
                    m_headers.clear();
                    continue ;
                }
                else {
                    ParseHeader(line);
                }
            }
            ++m_lineNumber;
        }
        if (m_inputBuffer->GetLength() > 16384)
            throw HttpException(404); /* TODO line too long */
    }
    catch (const HttpException &e)
    {
        printf("Http exception (status = %d)\n", e.GetStatus());
    }
}

void ProtocolCodec::ParseRequest(std::string line)
{
    tHttpMethodsMap::const_iterator it;
    std::string method;
    std::string path;
    std::string httpVersion;
    size_t firstSep;
    size_t lastSep;

    firstSep = line.find_first_of(' ');
    lastSep = line.find_last_of(' ');
    if (firstSep == lastSep || firstSep == std::string::npos)
    {
        throw HttpException(500); /* TODO invalid request */
    }
    method = line.substr(0, firstSep);
    path = line.substr(firstSep + 1, lastSep - firstSep - 1);
    httpVersion = line.substr(lastSep + 1);

    it = m_supportedMethods.find(method);
    if (it == m_supportedMethods.end())
    {
        throw HttpException(500); /* TODO unsupported method */
    }

    m_method = it->second;
    m_path = path;
    m_httpVersion = httpVersion;

    printf("Method: %s\n", method.c_str());
    printf("Path: %s\n", path.c_str());
}

static void ltrim(std::string &s)
{
    size_t i;

    for (i = 0; i < s.size(); ++i)
    {
        if (!std::isspace(s[i]))
            break ;
    }
    s.erase(0, i);
}

void ProtocolCodec::ParseHeader(std::string line)
{
    size_t sep;
    std::string key, value;

    sep = line.find_first_of(':');
    if (sep == std::string::npos)
    {
        /* TODO */
    }
    key = line.substr(0, sep);
    value = line.substr(sep + 1);

    ltrim(value);

    m_headers.insert(std::make_pair(key, value));
}

void ProtocolCodec::EncodeResponse(DataBuffer *buffer, std::string httpVersion, int status, std::string message)
{
    std::stringstream ss;

    ss << httpVersion << " " << status << " " << message << "\r\n";
    buffer->PutString(ss.str());
}

void ProtocolCodec::EncodeHeaders(DataBuffer *buffer, const tHttpHeaderMap &headers)
{
    std::stringstream ss;
    tHttpHeaderMap::const_iterator it;

    for (it = headers.begin(); it != headers.end(); ++it)
    {
        ss << it->first << ": " << it->second << "\r\n";
    }
    buffer->PutString(ss.str());
}

void ProtocolCodec::DispatchRequest()
{
    HttpRequest request;
    HttpResponse response;
    tHttpHeaderMap headers;
    DataBuffer *responseBody;

    request.SetMethod(m_method);
    request.SetPath(m_path);

    try {
        m_requestHandler->HandleRequest(&request, &response);
    }
    catch (const HttpException &e)
    {
        printf("Http exception\n");
    }
    catch (const std::exception &e)
    {
        printf("Generic exception: %s\n", e.what());
    }
    catch (...)
    {
        printf("Unknown exception\n");
    }

    const tHttpHeaderMap &responseHeaders = response.GetHeaders();
    headers.insert(responseHeaders.begin(), responseHeaders.end());

    EncodeResponse(m_outputBuffer, m_httpVersion, response.GetStatus(), response.GetStatusMessage());

    responseBody = response.GetOutputBuffer();
    if (!response.IsExternal()) {
        headers["Transfer-Encoding"] = "chunked";
    }

    EncodeHeaders(m_outputBuffer, headers);

    if (!response.IsExternal()) {
        m_outputBuffer->PutString("\r\n");

        EncodeChunk(m_outputBuffer, responseBody);
        EncodeChunk(m_outputBuffer, NULL);
    }
}

template <typename T>
std::string to_base(T val, int base = 10)
{
    static const char *digits = "0123456789abcdef";
    std::string s;

    do {
        s.insert(s.begin(), digits[val % base]);
        val /= base;
    } while (val > 0);
    return s;
}

void ProtocolCodec::EncodeChunk(DataBuffer *buffer, DataBuffer *chunk)
{
    size_t chunkSize;

    chunkSize = 0;
    if (chunk)
        chunkSize = chunk->GetLength();

    buffer->PutString(to_base(chunkSize, 16));
    buffer->PutString("\r\n");

    if (chunk)
        buffer->AddBuffer(chunk);
    buffer->PutString("\r\n");
}

ProtocolCodec::HttpRequest::HttpRequest()
{
}

ProtocolCodec::HttpRequest::~HttpRequest()
{
}

void ProtocolCodec::HttpRequest::SetMethod(HttpMethod method)
{
    m_method = method;
}

void ProtocolCodec::HttpRequest::SetPath(std::string path)
{
    m_rawPath = path;
}

ProtocolCodec::HttpResponse::HttpResponse()
{
    m_status = 200;
    m_message = "Ok";
}

ProtocolCodec::HttpResponse::~HttpResponse()
{
}

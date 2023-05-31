//
// Created by Alexis Lavaud on 23/03/2023.
//

#include "HttpProtocolCodec.h"
#include <sstream>
#include "HttpClientHandler.h"
#include "../IO/DataBuffer.h"
#include "Request.h"
#include "Response.h"
#include "../IO/BufferEvent.h"
#include "AsyncRequestHandler.h"
#include "../string_utils.hpp"
#include "HttpException.h"
#include <stdio.h>

HttpProtocolCodec::HttpProtocolCodec(HttpClientHandler *handler, DataBuffer *input, DataBuffer *output)
        : m_handler(handler), m_inputBuffer(input), m_outputBuffer(output), m_bufferEvent(handler->GetBufferEvent())
{
    m_requestHeaderParsed = false;
    m_asyncHandler = NULL;
    m_headersSent = false;
    m_pendingFinalizeResponse = false;

    m_methods.insert(std::make_pair("GET", kHttpMethod_Get));
    m_methods.insert(std::make_pair("POST", kHttpMethod_Post));
    m_methods.insert(std::make_pair("PUT", kHttpMethod_Put));
    m_methods.insert(std::make_pair("PATCH", kHttpMethod_Patch));
    m_methods.insert(std::make_pair("DELETE", kHttpMethod_Delete));
    m_methods.insert(std::make_pair("HEAD", kHttpMethod_Head));
    m_methods.insert(std::make_pair("OPTIONS", kHttpMethod_Options));
}

HttpProtocolCodec::~HttpProtocolCodec()
{
    delete m_asyncHandler;
}

void HttpProtocolCodec::ProcessDataInput()
{
    std::string line;
    bool dispatchRequest;

    printf("Async Handler %p:%p\n", this, m_asyncHandler);
    if (!m_asyncHandler) {
        dispatchRequest = false;
        while (m_inputBuffer->Readln(line)) {
            try {
                if (!m_requestHeaderParsed) {
                    ParseRequestHeader(line);
                    m_requestHeaderParsed = true;

                    if (HttpVersion::kHttpVersion_1_0 == m_httpVersion) {
                        dispatchRequest = true;
                    }
                } else {
                    if (line.empty()) {
                        if (m_headers.find("Content-Length") != m_headers.end()) {
                            std::string contentLength = m_headers["Content-Length"];
                            size_t i;

                            for (i = 0; i < contentLength.size(); ++i) {
                                if (!std::isdigit(contentLength[i]) || i > 10) {
                                    throw HttpException(HttpStatusCode::BadRequest);
                                }
                            }

                            m_bufferEvent->Enable(kEvent_Read | kEvent_Write);
                        }
                        else
                        {
                            /* Disable reading while processing the request */
                            m_bufferEvent->Enable(kEvent_Write);

                            if (m_method == kHttpMethod_Post ||
                                m_method == kHttpMethod_Patch ||
                                m_method == kHttpMethod_Put) {
                                throw HttpException(HttpStatusCode::LengthRequired);
                            }
                        }
                        dispatchRequest = true;
                    } else
                        ParseHeader(line);
                }
            }
            catch (const HttpException &e) {
                m_responseMessage = e.GetStatus().GetStatusMessage();
                m_responseStatus = e.GetStatus().GetStatusCode();
                m_httpVersion = HttpVersion(1, 0);
                m_responseHeaders.clear();
                m_responseHeaders["Content-Length"] = "0";
                m_responseHeaders["Content-Type"] = "text/html; charset=utf-8";
                m_bufferEvent->Enable(kEvent_Write);
                WriteResponseHeader();
                m_handler->Disconnect(true);
                return;
            }
            if (dispatchRequest) {
                dispatchRequest = false;
                DispatchRequest();
                if (m_asyncHandler)
                    break ;
            }
        }
        if (m_inputBuffer->GetLength() >= m_inputBuffer->GetReadHighWatermark()) {
            /* TODO handle veryyyy looong requests */
            // printf("Buffer full and no line could be read\n");
            //m_event->GetOutputBuffer()->PutString("Buffer full");
            // Disconnect(true);
        }
    }

    if (m_asyncHandler)
    {
        m_asyncHandler->OnDataIncoming(m_inputBuffer);
    }
}

void HttpProtocolCodec::OnOutputDrained()
{
    if (m_asyncHandler)
    {
        m_asyncHandler->OnOutputDrained();
    }
    if (m_pendingFinalizeResponse)
        FinalizeResponse();
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
        throw HttpException(HttpStatusCode::BadRequest);
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
    printf("[%s] %s\n", method.c_str(), rawPath.c_str());
    SetRequestHeader(method, rawPath, httpVersion);
}

void HttpProtocolCodec::SetRequestHeader(const std::string &method, const std::string &rawPath,
                                         const std::string &httpVersion)
{
    if (!m_httpVersion.Parse(httpVersion) || m_httpVersion.GetMajor() > 1)
    {
        throw HttpException(HttpStatusCode::BadRequest);
    }

    m_httpVersion = HttpVersion::Min(HttpVersion(1, 1), m_httpVersion);

    tHttpMethodsMap::const_iterator methodsIt = m_methods.find(method);
    if (methodsIt == m_methods.end())
        m_method = kHttpMethod_Invalid;
    else
        m_method = methodsIt->second;

    m_rawPath = rawPath;
    utils::trim(m_rawPath);
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
    utils::trim(key);
    utils::trim(value);
    m_headers.insert(std::make_pair(key, value));
}

void HttpProtocolCodec::WriteResponseHeader()
{
    std::stringstream ss;
    tHeaderMap::const_iterator it;

    if (m_headersSent)
        return ;
    m_headersSent = true;

    ss << m_httpVersion << " " << m_responseStatus << " " << m_responseMessage << "\r\n";
    for (it = m_responseHeaders.begin(); it != m_responseHeaders.end(); ++it)
    {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n";
    m_outputBuffer->PutString(ss.str());
}

void HttpProtocolCodec::DispatchRequest()
{
    Request request(m_handler, m_headers);
    Response response(m_handler);
    tHeaderMap headers;
    IAsyncRequestHandler *asyncHandler;
    DataBuffer *body;

    request.SetMethod(m_method);
    request.SetRawPath(m_rawPath);
    request.SetProtocolVersion(m_httpVersion);

    try {
        m_handler->HandleRequest(&request, &response);
    }
    catch (HttpException &e)
    {
        /* TODO unhandled exception */
        /* Exception shall be handled inside ServerHost */
        /* so we should never have to catch it here */
    }
    catch (...)
    {
        printf("Unexpected error\n");
    }

    m_responseHeaders.swap(response.GetHeaders());

    if (response.IsChunked())
    {
        m_responseHeaders["Transfer-Encoding"] = "chunked";
    }
    else
    {
        m_responseHeaders["Content-Length"] = utils::to_string(response.GetContentLength());
    }

    m_responseStatus = response.GetStatusCode();
    m_responseMessage = response.GetStatusMessage();

    m_asyncHandler = response.GetAsyncHandler();
    printf("Async Handler oàoà %p:%p\n", this, m_asyncHandler);
    m_chunked = response.IsChunked();

    body = response.GetBodyBuffer();
    if (body->GetLength() > 0)
    {
        WriteResponseHeader();
        Write(body);
        FinalizeResponse();
    }
}

void HttpProtocolCodec::FinalizeResponse()
{
    tHeaderMap::const_iterator it;

    if (!m_pendingFinalizeResponse)
    {
        if (!m_headersSent)
            WriteResponseHeader();

        if (m_chunked)
        {
            WriteChunkHeader(0);
            m_outputBuffer->PutString("\r\n");
        }
    }

    if (m_outputBuffer->GetLength() > 0)
    {
        m_pendingFinalizeResponse = true;
        return ;
    }

    delete m_asyncHandler;
    m_asyncHandler = NULL;

    if (HttpVersion::kHttpVersion_1_0 == m_httpVersion)
    {
        m_handler->Disconnect(true);
        return ;
    }
    else if (HttpVersion::kHttpVersion_1_1 == m_httpVersion)
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
    m_pendingFinalizeResponse = false;
    m_headersSent = false;

    /* Re-enable reads to receive the next request */
    m_bufferEvent->Enable(kEvent_Read);

    printf("Finalized\n");
}

void HttpProtocolCodec::Write(const void *data, size_t n)
{
    if (!m_headersSent)
        WriteResponseHeader();
    m_outputBuffer->Write(data, n);
}

void HttpProtocolCodec::Write(DataBuffer *buffer)
{
    if (!m_headersSent)
        WriteResponseHeader();

    if (m_chunked)
    {
        WriteChunkHeader(buffer->GetLength());
    }
    m_outputBuffer->AddBuffer(buffer);
    if (m_chunked)
    {
        m_outputBuffer->PutString("\r\n");
    }
}

void HttpProtocolCodec::WriteChunkHeader(size_t length)
{
    std::stringstream ss;

    ss << std::hex << length << "\r\n";
    m_outputBuffer->PutString(ss.str());
}

DataBuffer *HttpProtocolCodec::GetInputBuffer() const {
    return m_inputBuffer;
}

DataBuffer *HttpProtocolCodec::GetOutputBuffer() const {
    return m_outputBuffer;
}

void HttpProtocolCodec::AddHeader(const std::string &name, const std::string &value)
{
    size_t sep;

    /* Special case for the Status header which changes the status code and message */
    if ("Status" == name)
    {
        sep = value.find_first_of(' ');
        if (std::string::npos == sep)
            return ;
        std::string code = value.substr(0, sep);
        std::string status = value.substr(sep + 1);

        utils::trim(code);
        utils::trim(status);
        m_responseStatus = std::atoi(code.c_str());
        m_responseMessage = status;
    }
    else {
        m_responseHeaders[name] = value;
    }
}

//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "Response.h"
#include "../IO/DataBuffer.h"
#include "HttpStatusCode.h"
#include <fcntl.h>
#include <unistd.h>
#include "HttpProtocolCodec.h"
#include "HttpClientHandler.h"
#include "FileRequestHandler.h"

Response::Response()
{
    m_asyncHandler = NULL;
    m_contentLength = 0;
    m_chunked = false;
    SetStatus(HttpStatusCode::Ok);
}

Response::~Response()
{
}

void Response::SetStatusMessage(std::string message)
{
    m_message = message;
}

std::string Response::GetStatusMessage() const
{
    return m_message;
}

const Response::tHttpHeadersMap &Response::GetHeaders() const
{
    return m_headers;
}

Response::tHttpHeadersMap &Response::GetHeaders()
{
    return m_headers;
}

void Response::AddHeader(std::string key, std::string value)
{
    m_headers.insert(std::make_pair(key, value));
}

void Response::SetStatus(const HttpStatusCode &status) {
    m_message = status.GetStatusMessage();
    m_status = status.GetStatusCode();
}

int Response::GetStatusCode() const {
    return m_status;
}

void Response::SetStatusCode(int status) {
    m_status = status;
}

void Response::SetAsyncHandler(IAsyncRequestHandler *asyncHandler)
{
    m_asyncHandler = asyncHandler;
}

IAsyncRequestHandler *Response::GetAsyncHandler() const
{
    return m_asyncHandler;
}

void Response::SendFile(const std::string &path)
{
    FileRequestHandler *handler;
    int fd;

    fd = open(path.c_str(), O_RDONLY);
    if (fd < 0)
    {
        printf("404\n");
        return ;
    }
    if (NULL == (handler = FileRequestHandler::Create(m_clientHandler->GetEventLoop(), this, fd)))
    {
        close(fd);
        return ; /* TODO ???? Which error code should we use ? 500 ? */
    }
    SetAsyncHandler(handler);
}

HttpProtocolCodec *Response::GetHttpCodec() const
{
    return m_codec;
}

void Response::SetContentLength(size_t length) {
    m_contentLength = length;
}

size_t Response::GetContentLength() const {
    return m_contentLength;
}

void Response::SetChunked(bool chunked) {
    m_chunked = chunked;
}

bool Response::IsChunked() const {
    return m_chunked;
}

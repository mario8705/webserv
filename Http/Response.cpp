//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "Response.h"
#include "../IO/DataBuffer.h"
#include "HttpStatusCode.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "HttpProtocolCodec.h"
#include "HttpClientHandler.h"
#include "FileRequestHandler.h"
#include "../Webserv.h"
#include "../MimeDatabase.h"

Response::~Response()
{
    delete m_body;
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

bool Response::SendFile(const std::string &path)
{
    struct stat st;

    if (stat(path.c_str(), &st) < 0)
        return false;
    return SendFile(path, st.st_size);
}

bool Response::SendFile(const std::string &path, size_t length)
{
    int fd;

    fd = open(path.c_str(), O_RDONLY);
    if (fd < 0)
        return false;
    if (m_headers.find("Content-Type") == m_headers.end())
        AddHeader("Content-Type", Webserv::GetInstance()->GetMimeDatabase()->GetMimeType(path));
    SetAsyncHandler(new FileRequestHandler(m_clientHandler->GetEventLoop(), this, fd, length));
    return true;
}

HttpProtocolCodec *Response::GetHttpCodec() const
{
    return m_codec;
}

void Response::SetContentLength(size_t length) {
    m_contentLength = length;
}

size_t Response::GetContentLength() const {
    size_t bodyLength;

    bodyLength = m_body->GetLength();
    if (bodyLength > 0)
        return bodyLength;
    return m_contentLength;
}

void Response::SetChunked(bool chunked) {
    m_chunked = chunked;
}

bool Response::IsChunked() const {
    return m_chunked;
}

int Response::Write(const std::string &str)
{
    return Write(str.c_str(), str.size());
}

int Response::Write(const void *data, size_t n)
{
    return m_body->Write(data, n);
}

DataBuffer *Response::GetBodyBuffer() const
{
    return m_body;
}

Response::Response(HttpClientHandler *clientHandler)
    : m_clientHandler(clientHandler)
{
    m_codec = clientHandler->GetProtocolCodec();
    m_asyncHandler = NULL;
    m_contentLength = 0;
    m_status = 200;
    m_chunked = false;
    m_body = new DataBuffer;
    SetStatus(HttpStatusCode::Ok);
}

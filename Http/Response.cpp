//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "Response.h"
#include "../IO/DataBuffer.h"
#include "HttpStatusCode.h"

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

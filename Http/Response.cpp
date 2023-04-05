//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "Response.h"
#include "../IO/DataBuffer.h"
#include "HttpStatusCode.h"

Response::Response()
{
    m_outputBuffer = new DataBuffer;
}

Response::~Response()
{
    delete m_outputBuffer;
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

DataBuffer *Response::GetOutputBuffer() const
{
    return m_outputBuffer;
}

void Response::SetExternal(bool external)
{
    m_external = external;
}

bool Response::IsExternal() const
{
    return m_external;
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

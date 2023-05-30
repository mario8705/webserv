//
// Created by Alexis Lavaud on 05/04/2023.
//

#include "HttpStatusCode.h"

HttpStatusCode::HttpStatusCode(int status, const char *message) throw()
    : m_status(status), m_statusMessage(message)
{
}

HttpStatusCode::~HttpStatusCode()
{
}

int HttpStatusCode::GetStatusCode() const
{
    return m_status;
}

const char *HttpStatusCode::GetStatusMessage() const
{
    return m_statusMessage;
}

HttpStatusCode HttpStatusCode::Ok(200, "Ok");
HttpStatusCode HttpStatusCode::NotFound(404, "Not Found");

HttpStatusCode HttpStatusCode::MethodNotAllowed(405, "Method Not Allowed");
HttpStatusCode HttpStatusCode::RequestURITooLong(414, "Request-URI Too Long");


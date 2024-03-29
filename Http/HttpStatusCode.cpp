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

HttpStatusCode HttpStatusCode::Forbidden(403, "Forbidden");
HttpStatusCode HttpStatusCode::NotFound(404, "Not Found");

HttpStatusCode HttpStatusCode::BadRequest(400, "Bad Request");
HttpStatusCode HttpStatusCode::MethodNotAllowed(405, "Method Not Allowed");
HttpStatusCode HttpStatusCode::LengthRequired(411, "Length Required");
HttpStatusCode HttpStatusCode::RequestEntityTooLarge(413, "Request Entity Too Large");
HttpStatusCode HttpStatusCode::RequestURITooLong(414, "Request-URI Too Long");


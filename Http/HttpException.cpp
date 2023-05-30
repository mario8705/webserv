//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "HttpException.h"

HttpException::HttpException(const HttpStatusCode &code) throw()
    : m_statusCode(code)
{
}

HttpException::~HttpException() throw()
{
}

const HttpStatusCode &HttpException::GetStatus() const
{
    return m_statusCode;
}

const char *HttpException::what() const throw() {
    return "HttpException"; /* TODO lol ? */
}

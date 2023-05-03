//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "HttpException.h"

HttpException::HttpException(int status)
    : m_status(status)
{
}

HttpException::~HttpException()
{
}

int HttpException::GetStatus() const
{
    return m_status;
}

std::string HttpException::GetStatusMessage() const
{
    return m_message;
}

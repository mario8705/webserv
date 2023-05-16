//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "HttpException.h"

HttpException::HttpException(int status) throw()
    : m_status(status)
{
}

HttpException::~HttpException() throw()
{
}

int HttpException::GetStatus() const
{
    return m_status;
}

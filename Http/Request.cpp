//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "Request.h"

Request::Request()
{
}

Request::~Request()
{
}

HttpMethod Request::GetMethod() const
{
    return m_method;
}

std::string Request::GetRawPath() const
{
    return m_rawPath;
}

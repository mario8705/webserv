//
// Created by Alexis Lavaud on 05/05/2023.
//

#include "RegexException.h"

RegexException::RegexException(const std::string &message)
    : m_message(message)
{
}

RegexException::~RegexException() throw()
{
}

const char *RegexException::what() const throw()
{
    return m_message.c_str();
}

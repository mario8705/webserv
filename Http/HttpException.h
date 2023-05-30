//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_HTTPEXCEPTION_H
#define WEBSERV_HTTPEXCEPTION_H
#include <string>
#include <exception>
#include "HttpStatusCode.h"

class HttpException : public std::exception
{
public:
    explicit HttpException(const HttpStatusCode &code) throw();
    ~HttpException() throw();

    const HttpStatusCode &GetStatus() const;

    virtual const char *what() const throw();

    HttpStatusCode m_statusCode;
};


#endif //WEBSERV_HTTPEXCEPTION_H

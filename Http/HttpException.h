//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_HTTPEXCEPTION_H
#define WEBSERV_HTTPEXCEPTION_H
#include <string>

class HttpException
{
public:
    explicit HttpException(int status) throw();
    ~HttpException() throw();

    int GetStatus() const;

    int m_status;
};


#endif //WEBSERV_HTTPEXCEPTION_H

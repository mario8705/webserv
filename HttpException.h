//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_HTTPEXCEPTION_H
#define WEBSERV_HTTPEXCEPTION_H
#include <string>

class HttpException
{
public:
    HttpException(int status);
    ~HttpException();

    int GetStatus() const;
    std::string GetStatusMessage() const;

    int m_status;
    std::string m_message;
};


#endif //WEBSERV_HTTPEXCEPTION_H

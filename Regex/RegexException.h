//
// Created by Alexis Lavaud on 05/05/2023.
//

#ifndef WEBSERV_REGEXEXCEPTION_H
#define WEBSERV_REGEXEXCEPTION_H
#include <stdexcept>
#include <string>

class RegexException : public std::exception
{
public:
    explicit RegexException(const std::string &message);
    ~RegexException() throw();

    virtual const char *what() const throw();

private:
    std::string m_message;
};


#endif //WEBSERV_REGEXEXCEPTION_H

//
// Created by Alexis Lavaud on 31/05/2023.
//

#ifndef WEBSERV_HTTPREGISTRY_H
#define WEBSERV_HTTPREGISTRY_H
#include <string>
#include "HttpMethod.h"

class HttpRegistry
{
public:
    static HttpMethod GetMethodByName(const std::string &name);
    static std::string GetMethodName(HttpMethod method);
};


#endif //WEBSERV_HTTPREGISTRY_H

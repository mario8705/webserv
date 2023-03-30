//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_HTTPMETHOD_H
#define WEBSERV_HTTPMETHOD_H

enum HttpMethod
{
    kHttpMethod_Get = 0,
    kHttpMethod_Post,
    kHttpMethod_Put,
    kHttpMethod_Patch,
    kHttpMethod_Delete,
    kHttpMethod_Head,
    kHttpMethod_Options,

    kHttpMethod_Last,
};

#endif //WEBSERV_HTTPMETHOD_H

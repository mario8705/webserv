//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_HTTPMETHOD_H
#define WEBSERV_HTTPMETHOD_H

enum HttpMethod
{
    kHttpMethod_Get = 0x01,
    kHttpMethod_Post = 0x02,
    kHttpMethod_Put = 0x04,
    kHttpMethod_Patch = 0x08,
    kHttpMethod_Delete = 0x10,
    kHttpMethod_Head = 0x11,
    kHttpMethod_Options = 0x12,

    kHttpMethod_Invalid = 0,
};

#endif //WEBSERV_HTTPMETHOD_H

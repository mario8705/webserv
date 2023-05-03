//
// Created by Alexis Lavaud on 05/04/2023.
//

#ifndef WEBSERV_HTTPRESPONSE_H
#define WEBSERV_HTTPRESPONSE_H
#include "../Response.h"

class HttpProtocolCodec;

class HttpResponse : public Response
{
public:
    HttpResponse(HttpProtocolCodec *codec);
    ~HttpResponse();

    int Write(const void *data, size_t n);
    void End();
};


#endif //WEBSERV_HTTPRESPONSE_H

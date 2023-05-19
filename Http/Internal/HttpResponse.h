//
// Created by Alexis Lavaud on 05/04/2023.
//

#ifndef WEBSERV_HTTPRESPONSE_H
#define WEBSERV_HTTPRESPONSE_H
#include "../Response.h"

class HttpProtocolCodec;
class HttpClientHandler;

class HttpResponse : public Response
{
public:
    explicit HttpResponse(HttpClientHandler *clientHandler);
    ~HttpResponse();
};


#endif //WEBSERV_HTTPRESPONSE_H

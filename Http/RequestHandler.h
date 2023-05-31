//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_REQUESTHANDLER_H
#define WEBSERV_REQUESTHANDLER_H

class Request;
class Response;

class IRequestHandler
{
public:
    virtual ~IRequestHandler() {}

    virtual bool HandleRequest(Request *request, Response *response) = 0;
};

#endif //WEBSERV_REQUESTHANDLER_H

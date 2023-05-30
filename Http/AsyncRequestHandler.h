//
// Created by Alexis Lavaud on 06/04/2023.
//

#ifndef WEBSERV_ASYNCREQUESTHANDLER_H
#define WEBSERV_ASYNCREQUESTHANDLER_H

class DataBuffer;

class IAsyncRequestHandler
{
public:
    virtual ~IAsyncRequestHandler() {}

    virtual void OnOutputDrained() = 0;
    virtual void OnDataIncoming(DataBuffer *) = 0;
};

#endif //WEBSERV_ASYNCREQUESTHANDLER_H

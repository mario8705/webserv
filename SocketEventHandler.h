//
// Created by Alexis Lavaud on 22/03/2023.
//

#ifndef WEBSERV_SOCKETEVENTHANDLER_H
#define WEBSERV_SOCKETEVENTHANDLER_H

class DataBuffer;

enum EventType
{
    kEventType_EOF = 0,
    kEventType_Error = 1,
};

class ISocketEventHandler
{
public:
    virtual ~ISocketEventHandler() {}

    virtual void HandleRead(DataBuffer *) = 0;
    virtual void HandleWrite(DataBuffer *) = 0;
    virtual void HandleEvent(EventType) = 0;
};

#endif //WEBSERV_SOCKETEVENTHANDLER_H

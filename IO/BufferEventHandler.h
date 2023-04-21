//
// Created by Alexis Lavaud on 22/03/2023.
//

#ifndef WEBSERV_BUFFEREVENTHANDLER_H
#define WEBSERV_BUFFEREVENTHANDLER_H

class DataBuffer;

enum EventType
{
    kEventType_EOF = 0,
    kEventType_Error = 1,
    kEventType_Connected = 2,
};

class IBufferEventHandler
{
public:
    virtual ~IBufferEventHandler() {}

    virtual void HandleRead(DataBuffer *) = 0;
    virtual void HandleWrite(DataBuffer *) = 0;
    virtual void HandleEvent(EventType) = 0;
};

#endif //WEBSERV_BUFFEREVENTHANDLER_H

//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_SOCKETEVENT_H
#define WEBSERV_SOCKETEVENT_H
#include "IOEventBase.h"

class DataBuffer;
class IEventLoop;
class ISocketEventHandler;

class SocketEvent : public IOEventBase
{
public:
    SocketEvent(IEventLoop *eventLoop, ISocketEventHandler *handler, int fd);
    ~SocketEvent();

    void NotifyRead();
    void NotifyWrite();

    bool IsReadable() const;
    bool IsWritable() const;



    DataBuffer *GetInputBuffer() const;
    DataBuffer *GetOutputBuffer() const;

protected:
    IEventLoop *m_eventLoop;
    ISocketEventHandler *m_handler;
    DataBuffer *m_inputBuffer;
    DataBuffer *m_outputBuffer;
    bool m_readEnabled;
    bool m_writeEnabled;
};

#endif //WEBSERV_SOCKETEVENT_H

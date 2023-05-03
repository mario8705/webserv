//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_BUFFEREVENT_H
#define WEBSERV_BUFFEREVENT_H

#include <sys/socket.h>
#include "IOEventBase.h"

class DataBuffer;
class IEventLoop;
class IBufferEventHandler;

#define kEvent_Read 1
#define kEvent_Write 2

class BufferEvent : public IOEventBase
{
public:
    BufferEvent(IEventLoop *eventLoop, IBufferEventHandler *handler, int fd);
    ~BufferEvent();

    void NotifyRead();
    void NotifyWrite();

    bool IsReadable() const;
    bool IsWritable() const;

    /**
     *
     * @param flags kEvent_Read and/or kEvent_Write
     */
    void Enable(int flags);

    DataBuffer *GetInputBuffer() const;
    DataBuffer *GetOutputBuffer() const;

    static BufferEvent *Connect(IEventLoop *eventLoop, IBufferEventHandler *handler, sockaddr *addr, socklen_t addrlen);

protected:
    IEventLoop *m_eventLoop;
    IBufferEventHandler *m_handler;
    DataBuffer *m_inputBuffer;
    DataBuffer *m_outputBuffer;
    bool m_readEnabled;
    bool m_writeEnabled;
};

#endif //WEBSERV_BUFFEREVENT_H

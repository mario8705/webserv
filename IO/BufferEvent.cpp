//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "BufferEvent.h"
#include <unistd.h>
#include "BufferEventHandler.h"
#include "DataBuffer.h"
#include "EventLoop.h"

BufferEvent::BufferEvent(IEventLoop *eventLoop, IBufferEventHandler *handler, int fd)
    : IOEventBase(fd), m_eventLoop(eventLoop), m_handler(handler)
{
    m_inputBuffer = new DataBuffer;
    m_outputBuffer = new DataBuffer;
    m_readEnabled = false;
    m_writeEnabled = false;
    m_eventLoop->RegisterEvent(this);
}

BufferEvent::~BufferEvent()
{
    m_eventLoop->UnregisterEvent(this);
    delete m_inputBuffer;
    delete m_outputBuffer;
    ::close(m_fd);
}

void BufferEvent::NotifyRead() {
    int n;

    if ((n = m_inputBuffer->Read(m_fd)) <= 0) {
        if (0 == n)
            m_handler->HandleEvent(kEventType_EOF);
        else
            m_handler->HandleEvent(kEventType_Error);
    } else {
        m_handler->HandleRead(m_inputBuffer);
    }
}

void BufferEvent::NotifyWrite() {
    int n;

    if (m_outputBuffer->GetLength() > 0) {
        if ((n = m_outputBuffer->Write(m_fd)) <= 0) {
            if (0 == n)
                m_handler->HandleEvent(kEventType_EOF);
            else
                m_handler->HandleEvent(kEventType_Error);
        } else {
            m_handler->HandleWrite(m_outputBuffer);
        }
    }
}

bool BufferEvent::IsReadable() const
{
    return m_readEnabled && (m_inputBuffer->GetLength() < m_inputBuffer->GetReadHighWatermark());
}

bool BufferEvent::IsWritable() const
{
    return m_writeEnabled && (m_outputBuffer->GetLength() > 0);
}

DataBuffer *BufferEvent::GetInputBuffer() const {
    return m_inputBuffer;
}

DataBuffer *BufferEvent::GetOutputBuffer() const {
    return m_outputBuffer;
}

void BufferEvent::Enable(int flags)
{
    m_readEnabled = (flags & kEvent_Read) != 0;
    m_writeEnabled = (flags & kEvent_Write) != 0;

    if (m_readEnabled && m_inputBuffer->GetLength() > 0)
    {
        /* TODO add kEvent_Read to pending events */
        /**
         * We need to trigger a read event if there is unread data in
         * the input buffer, otherwise the client might be stuck waiting for a response
         * and the response stuck waiting for data to trigger the read.
         * We avoid that by manually triggered a read event to force the handlers
         * to process left-over data in the buffer.
         */
    }
}

BufferEvent *
BufferEvent::Connect(IEventLoop *eventLoop, IBufferEventHandler *handler, sockaddr *addr, socklen_t addrlen) {
    int fd;

    fd = socket(addr->sa_family, SOCK_STREAM, 0);
    if (fd < 0)
    {
        return NULL;
    }

    if (connect(fd, addr, addrlen) < 0)
    {
        close(fd);
        return NULL;
    }

    return new BufferEvent(eventLoop, handler, fd);
}

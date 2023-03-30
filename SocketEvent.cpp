//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "SocketEvent.h"
#include <unistd.h>
#include "DataBuffer.h"
#include "SocketEventHandler.h"
#include "EventLoop.h"

SocketEvent::SocketEvent(IEventLoop *eventLoop, ISocketEventHandler *handler, int fd)
    : IOEventBase(fd), m_eventLoop(eventLoop), m_handler(handler)
{
    m_inputBuffer = new DataBuffer;
    m_outputBuffer = new DataBuffer;
    m_readEnabled = true;
    m_writeEnabled = true;
    m_eventLoop->RegisterEvent(this);
}

SocketEvent::~SocketEvent()
{
    m_eventLoop->UnregisterEvent(this);
    delete m_inputBuffer;
    delete m_outputBuffer;
    ::close(m_fd);
}

void SocketEvent::NotifyRead() {
    int n;

    if ((n = m_inputBuffer->Receive(m_fd)) <= 0) {
        if (0 == n)
            m_handler->HandleEvent(kEventType_EOF);
        else
            m_handler->HandleEvent(kEventType_Error);
    } else {
        m_handler->HandleRead(m_inputBuffer);
    }
}

void SocketEvent::NotifyWrite() {
    int n;

    if (m_outputBuffer->GetLength() > 0) {
        if ((n = m_outputBuffer->Send(m_fd)) <= 0) {
            if (0 == n)
                m_handler->HandleEvent(kEventType_EOF);
            else
                m_handler->HandleEvent(kEventType_Error);
        } else {
            m_handler->HandleWrite(m_outputBuffer);
        }
    }
}

bool SocketEvent::IsReadable() const
{
    return m_readEnabled && (m_inputBuffer->GetLength() < m_inputBuffer->GetReadHighWatermark());
}

bool SocketEvent::IsWritable() const
{
    return m_writeEnabled && (m_outputBuffer->GetLength() > 0);
}

DataBuffer *SocketEvent::GetInputBuffer() const {
    return m_inputBuffer;
}

DataBuffer *SocketEvent::GetOutputBuffer() const {
    return m_outputBuffer;
}

//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "FileEvent.h"
#include <unistd.h>
#include "EventLoop.h"
#include "DataBuffer.h"

FileEvent::FileEvent(IEventLoop* eventLoop, int fd)
        : IOEventBase(fd), m_eventLoop(eventLoop)
{
    m_bufPos = 0;
    m_bufLen = 0;
    m_errorFlag = false;
    m_eof = false;

    m_eventLoop->RegisterEvent(this);
}

FileEvent::~FileEvent()
{
    m_eventLoop->UnregisterEvent(this);
    ::close(m_fd);
}

void FileEvent::HandleReadEvent()
{
    ssize_t n;
    ssize_t len;

    if (m_errorFlag)
        return ;
    if (m_bufPos <= m_bufLen)
    {
        n = read(m_fd, m_buffer, sizeof(m_buffer));
        printf("Reading...\n");
        if (n < 0)
        {
            m_errorFlag = true;
            return ;
        }
        if (n == 0)
            m_eof = true;
        m_bufPos = 0;
        m_bufLen = n;
        /* TODO handle read of size 0 as EOF */
    }
}

bool FileEvent::IsReadable() const
{
    return !m_eof && !m_errorFlag;
}

void FileEvent::WriteToBuffer(DataBuffer *buf)
{
    int n;

    n = buf->Write(m_buffer + m_bufPos, m_bufLen - m_bufPos);
    m_bufPos += n;
}

bool FileEvent::HasDataAvailable() const
{
    return m_bufLen > m_bufPos;
}

bool FileEvent::IsEOF() const
{
    return m_eof;
}
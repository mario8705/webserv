//
// Created by Alexis Lavaud on 15/03/2023.
//

#include "DataBuffer.h"
#include <sys/socket.h>

DataBuffer::DataBuffer(size_t capacity)
        : m_capacity(capacity)
{
    m_buffer = new char[capacity];
    m_readPos = 0;
    m_writePos = 0;
}

DataBuffer::~DataBuffer()
{
    delete[] m_buffer;
}

int DataBuffer::Receive(int fd)
{
    size_t avail;
    ssize_t n;

    avail = GetAvailableSpace();
    n = recv(fd, m_buffer + m_writePos, avail, 0);
    if (n > 0)
    {
        m_writePos += n;
    }
    if (m_writePos == m_capacity)
        m_writePos = 0;
    return (int)n;
}

bool DataBuffer::GetLine(std::string &line)
{
    size_t i;
    size_t len;
    bool nl;

    nl = false;
    line.clear();
    for (i = m_readPos, len = 0; i != m_writePos && !nl; )
    {
        if (m_buffer[i] == '\n')
            nl = true;
        else {
            ++len;
            line += m_buffer[i];
        }

        if (++i == m_capacity)
            i = 0;
    }

    if (nl)
    {
        if (line.size() > 0 && line[line.size() - 1] == '\r')
            line.resize(line.size() - 1);
        printf("i = %d, readpos = %d\n", i, m_readPos);
        m_readPos = i;
    }
    return nl;
}

int DataBuffer::Get()
{
    int ch;

    ch = -1;
    if (m_readPos != m_writePos)
    {
        ch = m_buffer[m_readPos++];
        if (m_readPos == m_capacity)
            m_readPos = 0;
    }
    return ch;
}

int DataBuffer::Put(int ch)
{
    if (m_readPos == ((m_writePos + 1) % m_capacity))
        return -1;
    m_buffer[m_writePos++] = (char)ch;
    if (m_writePos == m_capacity)
        m_writePos = 0;
    return 0;
}

int DataBuffer::Put(std::string s)
{
    size_t i;

    for (i = 0; i < s.size(); ++i)
    {
        if (Put(s[i]) < 0)
            return -1;
    }
    return 0;
}

size_t DataBuffer::GetAvailableSpace() const
{
    if (m_readPos <= m_writePos) {
        return m_capacity - m_writePos;
    }
    return m_readPos - m_writePos;
}
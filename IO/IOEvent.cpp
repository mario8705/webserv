//
// Created by Alexis Lavaud on 05/05/2023.
//

#include "IOEvent.h"
#include <unistd.h>

IOEvent::IOEvent(int fd)
    : m_fd(fd)
{
}

IOEvent::~IOEvent()
{
    ::close(m_fd);
}

int IOEvent::GetFileDescriptor() const
{
    return m_fd;
}

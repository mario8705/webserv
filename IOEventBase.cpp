//
// Created by alavaud on 3/14/23.
//

#include "IOEventBase.h"

IOEventBase::~IOEventBase()
{
}

void IOEventBase::HandleReadEvent()
{
}

void IOEventBase::HandleWriteEvent()
{
}

bool IOEventBase::IsWritable() const
{
    return false;
}

int IOEventBase::GetFileDescriptor() const
{
    return m_fd;
}

IOEventBase::IOEventBase(int fd)
    : m_fd(fd)
{
}

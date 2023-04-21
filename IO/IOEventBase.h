//
// Created by alavaud on 3/14/23.
//

#ifndef WEBSERV_IOEVENTBASE_H
#define WEBSERV_IOEVENTBASE_H
#include "IOEvent.h"

class IOEventBase : public IIOEvent
{
public:
    virtual ~IOEventBase();

    void NotifyRead();
    void NotifyWrite();

    bool IsReadable() const;
    bool IsWritable() const;

    int GetFileDescriptor() const;

protected:
    explicit IOEventBase(int fd);

    int m_fd;
};


#endif //WEBSERV_IOEVENTBASE_H

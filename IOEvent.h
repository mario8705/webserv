//
// Created by alavaud on 3/13/23.
//

#ifndef WEBSERV_IOEVENT_H
#define WEBSERV_IOEVENT_H

class IIOEvent
{
public:
    virtual ~IIOEvent() {}

    virtual void HandleReadEvent() = 0;
    virtual void HandleWriteEvent() = 0;

    virtual bool IsWritable() const = 0;

    virtual int GetFileDescriptor() const = 0;
};

#endif //WEBSERV_IOEVENT_H
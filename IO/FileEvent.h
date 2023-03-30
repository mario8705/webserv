//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_FILEEVENT_H
#define WEBSERV_FILEEVENT_H
#include "IOEventBase.h"

class IEventLoop;
class DataBuffer;

class FileEvent : public IOEventBase
{
public:
    FileEvent(IEventLoop* eventLoop, int fd);
    virtual ~FileEvent();

    void HandleReadEvent();

    bool IsReadable() const;

    void WriteToBuffer(DataBuffer *buf);

    bool HasDataAvailable() const;
    bool IsEOF() const;

private:
    IEventLoop *m_eventLoop;
    char m_buffer[1024];
    int m_bufPos;
    int m_bufLen;
    bool m_errorFlag;
    bool m_eof;
};

#endif //WEBSERV_FILEEVENT_H

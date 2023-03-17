//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_SOCKETEVENT_H
#define WEBSERV_SOCKETEVENT_H
#include "IOEventBase.h"

class ServerHost;
class DataBuffer;
class ProtocolCodec;

class SocketEvent : public IOEventBase
{
public:
    SocketEvent(ServerHost *host, int fd);
    virtual ~SocketEvent();

    void HandleReadEvent();
    void HandleWriteEvent();

    bool IsReadable() const;
    bool IsWritable() const;

protected:
    ServerHost *m_host;
    DataBuffer *m_receiveBuffer;
    DataBuffer *m_outputBuffer;
    ProtocolCodec *m_protocolCodec;
};

#endif //WEBSERV_SOCKETEVENT_H

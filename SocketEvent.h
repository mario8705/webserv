//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_SOCKETEVENT_H
#define WEBSERV_SOCKETEVENT_H
#include "IOEventBase.h"
#include "RequestHandler.h"

class ServerHost;
class DataBuffer;
class ProtocolCodec;

class SocketEvent : public IOEventBase, public IRequestHandler
{
public:
    SocketEvent(ServerHost *host, int fd);
    virtual ~SocketEvent();

    void HandleReadEvent();
    void HandleWriteEvent();

    bool IsReadable() const;
    bool IsWritable() const;

    void HandleRequest(Request *request, Response *response);

protected:
    void HandleOutputDrained();

    ServerHost *m_host;
    DataBuffer *m_receiveBuffer;
    DataBuffer *m_outputBuffer;
    ProtocolCodec *m_protocolCodec;
    bool m_cgi;
};

#endif //WEBSERV_SOCKETEVENT_H

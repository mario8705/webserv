//
// Created by Alexis Lavaud on 22/03/2023.
//

#ifndef WEBSERV_HTTPCLIENTHANDLER_H
#define WEBSERV_HTTPCLIENTHANDLER_H
#include "../IO/BufferEventHandler.h"
#include "RequestHandler.h"
#include <string>

class ServerHost;
class BufferEvent;
class HttpProtocolCodec;
class IEventLoop;

class HttpClientHandler : public IBufferEventHandler, public IRequestHandler
{
public:
    HttpClientHandler(ServerHost *host, int fd);
    ~HttpClientHandler();

    void HandleRead(DataBuffer *buffer);
    void HandleWrite(DataBuffer *buffer);
    void HandleEvent(EventType type);

    void HandleRequest(Request *request, Response *response);

    void Disconnect(bool flush);

    BufferEvent *GetBufferEvent() const;
    HttpProtocolCodec *GetProtocolCodec() const;
    IEventLoop *GetEventLoop() const;

private:
    ServerHost *m_host;
    BufferEvent *m_event;
    HttpProtocolCodec *m_protocolCodec;
    bool m_disconnecting;
};


#endif //WEBSERV_HTTPCLIENTHANDLER_H

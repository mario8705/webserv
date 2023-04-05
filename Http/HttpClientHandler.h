//
// Created by Alexis Lavaud on 22/03/2023.
//

#ifndef WEBSERV_HTTPCLIENTHANDLER_H
#define WEBSERV_HTTPCLIENTHANDLER_H
#include "../Network/SocketEventHandler.h"
#include "RequestHandler.h"
#include <string>

class ServerHost;
class SocketEvent;
class HttpProtocolCodec;

class HttpClientHandler : public ISocketEventHandler, public IRequestHandler
{
public:
    HttpClientHandler(ServerHost *host, int fd);
    ~HttpClientHandler();

    void HandleRead(DataBuffer *buffer);
    void HandleWrite(DataBuffer *buffer);
    void HandleEvent(EventType type);

    void HandleRequest(Request *request, Response *response);

    void Disconnect(bool flush);

private:
    ServerHost *m_host;
    SocketEvent *m_event;
    HttpProtocolCodec *m_protocolCodec;
    bool m_disconnecting;
};


#endif //WEBSERV_HTTPCLIENTHANDLER_H

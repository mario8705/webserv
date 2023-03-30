//
// Created by Alexis Lavaud on 22/03/2023.
//

#ifndef WEBSERV_HTTPCLIENTHANDLER_H
#define WEBSERV_HTTPCLIENTHANDLER_H
#include "../Network/SocketEventHandler.h"
#include <string>

class ServerHost;
class SocketEvent;

class HttpClientHandler : public ISocketEventHandler
{
public:
    HttpClientHandler(ServerHost *host, int fd);
    ~HttpClientHandler();

    void HandleRead(DataBuffer *buffer);
    void HandleWrite(DataBuffer *buffer);
    void HandleEvent(EventType type);

    void Disconnect(bool flush);

private:
    void ProcessLine(const std::string &line);

    ServerHost *m_host;
    SocketEvent *m_event;
    bool m_disconnecting;
    int m_lineNumber;
};


#endif //WEBSERV_HTTPCLIENTHANDLER_H

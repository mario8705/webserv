//
// Created by Alexis Lavaud on 04/04/2023.
//

#ifndef WEBSERV_HTTPCLIENT_H
#define WEBSERV_HTTPCLIENT_H
#include <cstdint>
#include <string>

class IEventLoop;
class SocketEvent;

class HttpClient
{
public:
    HttpClient(IEventLoop *eventLoop, const char *host, uint16_t port);
    ~HttpClient();

    void DoRequest(const char *method, const char *path);

private:
    IEventLoop *m_eventLoop;
    SocketEvent *m_socketEvent;
    std::string m_host;
    uint16_t m_port;
};


#endif //WEBSERV_HTTPCLIENT_H

//
// Created by Alexis Lavaud on 04/04/2023.
//

#include "HttpClient.h"
#include "../IO/BufferEvent.h"

HttpClient::HttpClient(IEventLoop *eventLoop, const char *host, uint16_t port)
    : m_eventLoop(eventLoop), m_host(host), m_port(port)
{
    m_event = NULL;
}

HttpClient::~HttpClient()
{
    delete m_event;
}

void HttpClient::DoRequest(const char *method, const char *path)
{
    if (!m_event)
    {
    }
}

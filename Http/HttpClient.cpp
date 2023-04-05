//
// Created by Alexis Lavaud on 04/04/2023.
//

#include "HttpClient.h"

HttpClient::HttpClient(IEventLoop *eventLoop, const char *host, uint16_t port)
    : m_eventLoop(eventLoop), m_host(host), m_port(port)
{
    m_socketEvent = NULL;
}

HttpClient::~HttpClient()
{
    if (m_socketEvent)
    {
        delete m_socketEvent;
    }
}

void HttpClient::DoRequest(const char *method, const char *path)
{
    if (!m_socketEvent)
    {
    }
}

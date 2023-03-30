//
// Created by Alexis Lavaud on 22/03/2023.
//

#include "HttpClientHandler.h"
#include "SocketEvent.h"
#include "ServerHost.h"
#include "DataBuffer.h"
#include "HttpMethod.h"

namespace Http
{
    static void ParseHttpMethod(const std::string &line, HttpMethod &method, std::string &path, std::string &httpVersion)
    {

    }
}

HttpClientHandler::HttpClientHandler(ServerHost *host, int fd)
    : m_host(host)
{
    m_event = new SocketEvent(host->GetEventLoop(), this, fd);
    m_event->GetInputBuffer()->SetReadHighWatermark(16384);
    m_lineNumber = 0;
}

HttpClientHandler::~HttpClientHandler()
{
    delete m_event;
}

void HttpClientHandler::HandleRead(DataBuffer *buffer)
{
    std::string line;

    while (buffer->Readln(line))
    {
        ProcessLine(line);
    }
    if (buffer->GetLength() >= buffer->GetReadHighWatermark())
    {
        printf("Buffer full and no line could be read\n");
        m_event->GetOutputBuffer()->PutString("Buffer full");
        Disconnect(true);
    }
}

void HttpClientHandler::HandleWrite(DataBuffer *buffer)
{
    if (m_disconnecting && m_event->GetOutputBuffer()->GetLength() == 0)
    {
        m_host->DeferDeletion(this);
    }
}

void HttpClientHandler::HandleEvent(EventType type)
{
    Disconnect(false);
}

void HttpClientHandler::ProcessLine(const std::string &line)
{
    if (m_lineNumber == 0)
    {
        HttpMethod method;
        std::string path;
        std::string httpVersion;
        Http::ParseHttpMethod(line, method, path, httpVersion);
    }
    else
    {
        if (line.empty())
        {
            printf("Request !\n");
            m_event->GetOutputBuffer()->PutString("HTTP/1.1 200 Ok\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nHello");
            Disconnect(true);
        }
        else
        {

        }
    }
    ++m_lineNumber;
}

void HttpClientHandler::Disconnect(bool flush) {
    m_disconnecting = true;
    if (!flush || m_event->GetOutputBuffer()->GetLength() == 0) {
        m_host->DeferDeletion(this);
    }
}


//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "SocketEvent.h"
#include <unistd.h>
#include "DataBuffer.h"
#include "ServerHost.h"

SocketEvent::SocketEvent(ServerHost *host, int fd)
        : IOEventBase(fd), m_host(host)
{
    m_receiveBuffer = new DataBuffer;
    m_outputBuffer = new DataBuffer;
}

SocketEvent::~SocketEvent()
{
    delete m_receiveBuffer;
    delete m_outputBuffer;
    ::close(m_fd);
}

void SocketEvent::HandleReadEvent()
{
    int n;
    std::string line;

    n = m_receiveBuffer->Receive(m_fd);
    if (n <= 0)
    {
        m_host->Disconnect(this);
    }
    else
    {
        while (m_receiveBuffer->Readln(line))
        {
            if (line.empty())
            {
                std::string html = "<h1>Welcome to webserv !</h1><p>Everything is working great :)</p>";
                std::string headers = "HTTP/1.1 200 Ok\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: " + std::to_string(html.size());
                headers += "\r\n\r\n";

                m_outputBuffer->Write(headers.c_str(), headers.size());
                m_outputBuffer->Write(html.c_str(), html.size());
            }
            printf("Got a line: %s\n", line.c_str());
        }
    }
}

void SocketEvent::HandleWriteEvent()
{
    m_outputBuffer->Send(m_fd);
}

bool SocketEvent::IsReadable() const
{
    return true;
}

bool SocketEvent::IsWritable() const
{
    return m_outputBuffer->GetLength() > 0;
}

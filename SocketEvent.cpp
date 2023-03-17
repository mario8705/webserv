//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "SocketEvent.h"
#include <unistd.h>
#include "DataBuffer.h"
#include "ServerHost.h"
#include "ProtocolCodec.h"

SocketEvent::SocketEvent(ServerHost *host, int fd)
        : IOEventBase(fd), m_host(host)
{
    m_receiveBuffer = new DataBuffer;
    m_outputBuffer = new DataBuffer;
    m_protocolCodec = new ProtocolCodec(this, m_receiveBuffer, m_outputBuffer);

    m_cgi = false;
}

SocketEvent::~SocketEvent()
{
    delete m_protocolCodec;
    delete m_receiveBuffer;
    delete m_outputBuffer;
    ::close(m_fd);
}

void SocketEvent::HandleReadEvent()
{
    int n;

    n = m_receiveBuffer->Receive(m_fd);
    if (n <= 0)
    {
        m_host->Disconnect(this);
    }
    else
    {
        m_protocolCodec->ProcessData();
    }
}

void SocketEvent::HandleWriteEvent()
{
    m_outputBuffer->Send(m_fd);

    if (m_outputBuffer->GetLength() == 0)
        HandleOutputDrained();
}

bool SocketEvent::IsReadable() const
{
    /* Limit the receive buffer size to 16KiB */
    return m_receiveBuffer->GetLength() <= 16384;
}

bool SocketEvent::IsWritable() const
{
    return m_outputBuffer->GetLength() > 0;
}

void SocketEvent::HandleRequest(Request *request, Response *response)
{
    response->SetStatus(200);
    response->SetStatusMessage("Ok");
    response->SetExternal(true);

    m_cgi = true;

    //response->AddHeader("Content-Type", "text/html; charset=UTF-8");
    //response->GetOutputBuffer()->PutString(request->GetRawPath());
}

void SocketEvent::HandleOutputDrained()
{
    pid_t p;
    std::vector<std::string> args;

    if (m_cgi)
    {
        p = ::fork();
        if (p == 0) {
            if (::dup2(m_fd, 1) < 0) {
                /* TODO */
            }
            if (::execl("/usr/local/bin/php-cgi", "php-cgi", "index.php", NULL) < 0) {
                perror("execl");
            }
            exit(1);
        }
        m_host->Disconnect(this);
    }
}
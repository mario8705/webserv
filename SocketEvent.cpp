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
    m_protocolCodec = new ProtocolCodec(m_receiveBuffer, m_outputBuffer);
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
    std::string line;

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

//
// Created by Alexis Lavaud on 22/03/2023.
//

#include "HttpClientHandler.h"
#include "../IO/BufferEvent.h"
#include "../ServerHost.h"
#include "../IO/DataBuffer.h"
#include "HttpProtocolCodec.h"
#include "HttpStatusCode.h"

HttpClientHandler::HttpClientHandler(ServerHost *host, int fd)
    : m_host(host), m_disconnecting(false)
{
    m_event = new BufferEvent(host->GetEventLoop(), this, fd);
    /* TODO doesn't work lol please fix (later in v2 ahha) */
    /* m_event->GetInputBuffer()->SetReadHighWatermark(16384); */
    m_event->GetInputBuffer()->SetReadHighWatermark(16777216);
    m_event->Enable(kEvent_Read);
    m_protocolCodec = new HttpProtocolCodec(this, m_event->GetInputBuffer(), m_event->GetOutputBuffer());
}

HttpClientHandler::~HttpClientHandler()
{
    delete m_protocolCodec;
    delete m_event;
}

void HttpClientHandler::HandleRead(DataBuffer *buffer)
{
    m_protocolCodec->ProcessDataInput();
}

void HttpClientHandler::HandleWrite(DataBuffer *buffer) {
    if (m_disconnecting) {
        if (m_event->GetOutputBuffer()->GetLength() == 0)
            m_host->DeferDeletion(this);
    } else {
        m_protocolCodec->OnOutputDrained();
    }
}

void HttpClientHandler::HandleEvent(EventType type)
{
    Disconnect(false);
}

void HttpClientHandler::Disconnect(bool flush) {
    m_disconnecting = true;
    m_event->Enable(kEvent_Write);
    if (!flush || m_event->GetOutputBuffer()->GetLength() == 0) {
        m_host->DeferDeletion(this);
    }
}

void HttpClientHandler::HandleRequest(Request *request, Response *response)
{
    m_host->HandleRequest(request, response);
}

BufferEvent *HttpClientHandler::GetBufferEvent() const
{
    return m_event;
}

HttpProtocolCodec *HttpClientHandler::GetProtocolCodec() const
{
    return m_protocolCodec;
}

IEventLoop *HttpClientHandler::GetEventLoop() const
{
    return m_host->GetEventLoop();
}


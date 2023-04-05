//
// Created by Alexis Lavaud on 22/03/2023.
//

#include "HttpClientHandler.h"
#include "../Network/SocketEvent.h"
#include "../ServerHost.h"
#include "../IO/DataBuffer.h"
#include "HttpProtocolCodec.h"
#include "HttpStatusCode.h"

HttpClientHandler::HttpClientHandler(ServerHost *host, int fd)
    : m_host(host), m_disconnecting(false)
{
    m_event = new SocketEvent(host->GetEventLoop(), this, fd);
    m_event->GetInputBuffer()->SetReadHighWatermark(16384);
    m_protocolCodec = new HttpProtocolCodec(this);
}

HttpClientHandler::~HttpClientHandler()
{
    delete m_protocolCodec;
    delete m_event;
}

void HttpClientHandler::HandleRead(DataBuffer *buffer)
{
    m_protocolCodec->ProcessDataInput(buffer);
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

void HttpClientHandler::Disconnect(bool flush) {
    m_disconnecting = true;
    if (!flush || m_event->GetOutputBuffer()->GetLength() == 0) {
        m_host->DeferDeletion(this);
    }
}

#include <iostream>
#include "Request.h"
#include "Response.h"

static inline std::ostream &operator<<(std::ostream &os, HttpMethod method)
{
    static std::string kMethodsList[] = { "GET", "POST", "PUT" };
    os << kMethodsList[method];
    return os;
}

void HttpClientHandler::HandleRequest(Request *request, Response *response)
{
    std::cout << "Method: " << request->GetMethod() << std::endl;
    std::cout << "Path: " << request->GetRawPath() << std::endl;

    response->GetOutputBuffer()->PutString("<h1>Hello, World !</h1>");

    response->SetStatus(HttpStatusCode::NotFound);
    response->AddHeader("Content-Type", "text/html");
    response->AddHeader("Content-Length", std::to_string(response->GetOutputBuffer()->GetLength())); /* TODO warning C++11 */

    m_protocolCodec->EncodeResponse(m_event->GetOutputBuffer(), response);
    m_protocolCodec->FinalizeResponse();
}


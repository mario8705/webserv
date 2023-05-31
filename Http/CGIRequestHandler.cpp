//
// Created by Alexis Lavaud on 26/05/2023.
//

#include "CGIRequestHandler.h"
#include "../IO/BufferEvent.h"
#include "../IO/BufferEventHandler.h"
#include "../Cgi/CgiManager.hpp"
#include "../Http/Response.h"
#include "../Http/HttpProtocolCodec.h"
#include "../string_utils.hpp"
#include "../IO/DataBuffer.h"
#include "Request.h"

#include <stdio.h>

class PipeToBufferEvent : public IBufferEventHandler
{
public:
    PipeToBufferEvent(IEventLoop *eventLoop, int infd, Response *sink)
        : m_codec(sink->GetHttpCodec())
    {
        m_bev = new BufferEvent(eventLoop, this, infd);
        m_bev->Enable(kEvent_Read);
        m_headersReceived = false;
    }

    ~PipeToBufferEvent()
    {
        delete m_bev;
    }

    void HandleRead(DataBuffer *in)
    {
        std::string line;
        size_t sep;

        if (!m_headersReceived)
        {
            while (in->Readln(line))
            {
                if (line.empty())
                {
                    m_headersReceived = true;
                    break ;
                }
                if (std::string::npos != (sep = line.find_first_of(':')))
                {
                    std::string key, value;

                    key = line.substr(0, sep);
                    value = line.substr(sep + 1);
                    utils::to_lower(value);
                    m_codec->AddHeader(utils::trim(key), utils::trim(value));
                }
            }
        }
        if (m_headersReceived && in->GetLength() > 0)
        {
            m_codec->Write(in);
        }
    }

    void HandleWrite(DataBuffer *)
    {
    }

    void HandleEvent(EventType)
    {
        m_bev->Enable(0);
        m_codec->FinalizeResponse();
    }

    HttpProtocolCodec *m_codec;
    BufferEvent *m_bev;
    bool m_headersReceived;
};

class BufferEventToPipe : public IBufferEventHandler
{
public:
    BufferEventToPipe(IEventLoop *eventLoop, int outfd, size_t contentLength)
        : m_bytesLeft(contentLength)
    {
        m_bev = new BufferEvent(eventLoop, this, outfd);
        m_bev->Enable(kEvent_Write);
    }

    ~BufferEventToPipe()
    {
        delete m_bev;
    }

    void OnDataIncoming(DataBuffer *in)
    {
        if (m_bytesLeft == 0 || !m_bev)
            return ;

        /* TODO v2: only add up to bytesLeft */
        /* In practice no web client should send more than content length */
        if (in->GetLength() > m_bytesLeft)
            m_bytesLeft = 0;
        else
            m_bytesLeft -= in->GetLength();
        m_bev->GetOutputBuffer()->AddBuffer(in);
    }

    void HandleRead(DataBuffer *)
    {
    }

    void HandleEvent(EventType)
    {
        /* TODO handle event ? */
    }

    void HandleWrite(DataBuffer *out)
    {
        if (out->GetLength() == 0 && m_bytesLeft == 0)
        {
            delete m_bev;
            m_bev = NULL;
        }
    }

private:
    BufferEvent *m_bev;
    size_t m_bytesLeft;
};

void CGIRequestHandler::OnOutputDrained() {

}

CGIRequestHandler::CGIRequestHandler(IEventLoop *eventLoop, Request *request, Response *response, CgiManager *manager)
    : m_manager(manager)
{
    response->SetChunked(true);

    m_ptb = new PipeToBufferEvent(eventLoop, manager->GetMISO(),
                                  response);
    m_btp = new BufferEventToPipe(eventLoop, manager->GetMOSI(), request->GetContentLength());
}

CGIRequestHandler::~CGIRequestHandler()
{
    delete m_btp;
    delete m_ptb;
    delete m_manager;
}

void CGIRequestHandler::OnDataIncoming(DataBuffer *in)
{
    m_btp->OnDataIncoming(in);
}

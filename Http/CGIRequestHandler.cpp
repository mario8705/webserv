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
                    m_codec->AddHeader(utils::trim(key), utils::trim(value));
                }
            }
        }
        if (m_headersReceived && in->GetLength() > 0)
        {
            m_codec->Write(in);
        }
    }

    void HandleWrite(DataBuffer *buffer)
    {
    }

    void HandleEvent(EventType type)
    {
        m_bev->Enable(0);
        m_codec->FinalizeResponse();
    }

    HttpProtocolCodec *m_codec;
    BufferEvent *m_bev;
    bool m_headersReceived;
};

void CGIRequestHandler::OnOutputDrained() {

}

CGIRequestHandler::CGIRequestHandler(IEventLoop *eventLoop, Response *response, CgiManager *manager)
    : m_manager(manager)
{
    close(manager->GetMOSI());

    response->SetChunked(true);

    m_ptb = new PipeToBufferEvent(eventLoop, manager->GetMISO(),
                                  response);
}

CGIRequestHandler::~CGIRequestHandler()
{
    delete m_ptb;
    delete m_manager;
}
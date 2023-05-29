//
// Created by Alexis Lavaud on 06/04/2023.
//

#include "FileRequestHandler.h"
#include "../IO/BufferEvent.h"
#include "Response.h"
#include "HttpProtocolCodec.h"
#include "../IO/DataBuffer.h"
#include "HttpStatusCode.h"

FileRequestHandler::FileRequestHandler(IEventLoop *eventLoop, Response *response, int fd, size_t length)
        : m_codec(response->GetHttpCodec()), m_length(length)
{
    m_event = new BufferEvent(eventLoop, this, fd);
    m_event->Enable(kEvent_Read);
    m_event->GetInputBuffer()->SetReadHighWatermark(65536);

    response->SetContentLength(length);
    response->SetChunked(false);
}

FileRequestHandler::~FileRequestHandler()
{
    delete m_event;
}

void FileRequestHandler::HandleEvent(EventType type) {
    if (kEventType_EOF == type) {
        m_event->Enable(0);
        m_codec->FinalizeResponse();
    }
}

/**
 * Data is available in the file buffer.
 * @param buffer
 */
void FileRequestHandler::HandleRead(DataBuffer *buffer) {
    if (m_codec->GetOutputBuffer()->GetLength() < 65536)
        m_codec->Write(m_event->GetInputBuffer());
}

void FileRequestHandler::HandleWrite(DataBuffer *buffer) {

}

void FileRequestHandler::OnOutputDrained() {
    if (m_codec->GetOutputBuffer()->GetLength() < 65536)
        m_codec->Write(m_event->GetInputBuffer());
}

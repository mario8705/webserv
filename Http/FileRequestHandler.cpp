//
// Created by Alexis Lavaud on 06/04/2023.
//

#include "FileRequestHandler.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../IO/BufferEvent.h"
#include "Response.h"
#include "HttpProtocolCodec.h"
#include "../IO/DataBuffer.h"

FileRequestHandler::FileRequestHandler(IEventLoop *eventLoop, Response *response, int fd, size_t length)
        : m_codec(response->GetHttpCodec()), m_length(length)
{
    m_event = new BufferEvent(eventLoop, this, fd);
    m_event->Enable(kEvent_Read);
    m_event->GetInputBuffer()->SetReadHighWatermark(65536);

    response->AddHeader("Content-Type", "application/octet-stream");
    response->SetContentLength(length);
    response->SetChunked(false);
}

FileRequestHandler::~FileRequestHandler()
{
    printf("File handler deleted\n");
    delete m_event;
}

void FileRequestHandler::HandleEvent(EventType type) {
    if (kEventType_EOF == type) {
        m_event->Enable(0);
        m_codec->FinalizeResponse();
    }
}

void FileRequestHandler::HandleRead(DataBuffer *buffer) {
    m_codec->Write(m_event->GetInputBuffer());
    printf("Current input buffer length: %zu\n", m_event->GetInputBuffer()->GetLength());
}

void FileRequestHandler::HandleWrite(DataBuffer *buffer) {

}

FileRequestHandler *FileRequestHandler::Create(IEventLoop *eventLoop, Response *response, int fd) {
    struct stat st;

    if (fstat(fd, &st) < 0) {
        return NULL;
    }
    return new FileRequestHandler(eventLoop, response, fd, st.st_size);
}


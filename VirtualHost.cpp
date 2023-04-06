//
// Created by Alexis Lavaud on 05/04/2023.
//

#include "VirtualHost.h"
#include "ServerHost.h"

VirtualHost::VirtualHost(ServerHost *serverHost)
    : m_host(serverHost)
{
}

VirtualHost::~VirtualHost()
{
}

#include "Http/Request.h"
#include "Http/Response.h"
#include "IO/DataBuffer.h"
#include "Http/AsyncRequestHandler.h"
#include "IO/BufferEvent.h"
#include <sstream>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "Http/HttpProtocolCodec.h"
#include "IO/BufferEventHandler.h"

class FileRequestHandler : public IAsyncRequestHandler, public IBufferEventHandler
{
public:
    ~FileRequestHandler()
    {
        printf("FileRequestHandler deleted !!!!!\n");
        delete m_event;
    }

    void HandleRead(DataBuffer *buffer)
    {
        m_codec->Write(buffer);
    }

    void HandleWrite(DataBuffer *buffer)
    {
    }

    void HandleEvent(EventType type)
    {
        if (kEventType_EOF == type)
        {
            m_event->Enable(0);
            m_codec->FinalizeResponse();
        }
    }

    static FileRequestHandler *Create(IEventLoop *eventLoop, Response *response, int fd)
    {
        struct stat st;

        if (fstat(fd, &st) < 0)
        {
            return NULL;
        }

        return new FileRequestHandler(eventLoop, response, fd, st.st_size);
    }

private:
    FileRequestHandler(IEventLoop *eventLoop, Response *response, int fd, size_t length)
            : m_codec(response->GetHttpCodec()), m_length(length)
    {
        m_event = new BufferEvent(eventLoop, this, fd);
        m_event->Enable(kEvent_Read);

        response->AddHeader("Content-Type", "application/octet-stream");
        response->SetContentLength(length);
        response->SetChunked(false);
    }

    BufferEvent *m_event;
    HttpProtocolCodec *m_codec;
    size_t m_length;
};

void VirtualHost::HandleRequest(Request *request, Response *response)
{
    std::stringstream ss;
    FileRequestHandler *handler;

    int fd;

    fd = open("/dev/urandom", O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        perror("meh");
        return ;
    }

    if (NULL == (handler = FileRequestHandler::Create(m_host->GetEventLoop(), response, fd)))
    {
        /* TODO throw error */
        return ;
    }

    response->SetAsyncHandler(handler);
}

const VirtualHost::tServerNameList &VirtualHost::GetServerNames() const
{
    return m_serverNames;
}

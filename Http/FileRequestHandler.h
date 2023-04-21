//
// Created by Alexis Lavaud on 06/04/2023.
//

#ifndef WEBSERV_FILEREQUESTHANDLER_H
#define WEBSERV_FILEREQUESTHANDLER_H
#include "AsyncRequestHandler.h"
#include "../IO/BufferEventHandler.h"
#include <cstddef>

class IEventLoop;
class Response;
class BufferEvent;
class HttpProtocolCodec;

class FileRequestHandler : public IAsyncRequestHandler, public IBufferEventHandler
{
public:
    ~FileRequestHandler();

    void HandleRead(DataBuffer *buffer);
    void HandleWrite(DataBuffer *buffer);
    void HandleEvent(EventType type);

    void OnOutputDrained();

    static FileRequestHandler *Create(IEventLoop *eventLoop, Response *response, int fd);

private:
    FileRequestHandler(IEventLoop *eventLoop, Response *response, int fd, size_t length);

    BufferEvent *m_event;
    HttpProtocolCodec *m_codec;
    size_t m_length;
};

#endif //WEBSERV_FILEREQUESTHANDLER_H
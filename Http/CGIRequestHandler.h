//
// Created by Alexis Lavaud on 26/05/2023.
//

#ifndef WEBSERV_CGIREQUESTHANDLER_H
#define WEBSERV_CGIREQUESTHANDLER_H
#include "AsyncRequestHandler.h"

class CgiManager;
class BufferEvent;
class IEventLoop;
class Response;

class PipeToBufferEvent;
class BufferEventToPipe;

class CGIRequestHandler : public IAsyncRequestHandler
{
public:
    explicit CGIRequestHandler(IEventLoop *eventLoop, Response *response, CgiManager *manager);
    ~CGIRequestHandler();

    void OnOutputDrained();

private:
    CgiManager *m_manager;
    BufferEventToPipe *m_btp;
    PipeToBufferEvent *m_ptb;
};


#endif //WEBSERV_CGIREQUESTHANDLER_H

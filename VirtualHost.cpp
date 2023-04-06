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
#include "Http/FileRequestHandler.h"

#include <fcntl.h>
#include <unistd.h>

void VirtualHost::HandleRequest(Request *request, Response *response)
{
    std::stringstream ss;
    FileRequestHandler *handler;

    int fd;

    fd = open("/Users/alexislavaud/Downloads/The Batman [FR-EN] (2022).mkv", O_RDONLY | O_NONBLOCK);
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

//
// Created by Alexis Lavaud on 05/04/2023.
//

#include "VirtualHost.h"
#include "ServerHost.h"
#include "Http/HttpException.h"
#include "MountPoint.h"

VirtualHost::VirtualHost()
{
    m_rootMountPoint = new MountPoint(this, kRouteMatch_StartsWith, "/");
}

VirtualHost::~VirtualHost()
{
    delete m_rootMountPoint;
}

void VirtualHost::AddListenAddress(const NetworkAddress4 &addr)
{
    m_bindAddresses.push_back(addr);
}

#include "Http/Request.h"
#include "Http/Response.h"
#include "IO/DataBuffer.h"
#include "IO/BufferEvent.h"
#include <sstream>
#include "Http/FileRequestHandler.h"
#include "Http/URL.h"
#include "MountPoint.h"

void VirtualHost::HandleRequest(Request *request, Response *response)
{
    DispatchRequest(m_rootMountPoint, request, response);
}

const VirtualHost::tServerNameList &VirtualHost::GetServerNames() const
{
    return m_serverNames;
}

void VirtualHost::DispatchRequest(MountPoint *mountPoint, Request *request, Response *response)
{
    try
    {
        mountPoint->HandleRequest(request, response);
    }
    catch (HttpException &e)
    {
        if (!mountPoint->HandleException(request, response, &e))
        {
            printf("Unhandled exception, showing default error page !\n");
        }
    }
}
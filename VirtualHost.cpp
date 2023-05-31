//
// Created by Alexis Lavaud on 05/04/2023.
//

#include "VirtualHost.h"
#include <sstream>
#include "ServerHost.h"
#include "Http/HttpException.h"
#include "MountPoint.h"
#include "Http/Request.h"
#include "Http/Response.h"
#include "IO/DataBuffer.h"
#include "Http/FileRequestHandler.h"

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

bool VirtualHost::HandleRequest(Request *request, Response *response)
{
    DispatchRequest(m_rootMountPoint, request, response);
    return true;
}

const VirtualHost::tServerNameList &VirtualHost::GetServerNames() const
{
    return m_serverNames;
}

void VirtualHost::DispatchRequest(MountPoint *mountPoint, Request *request, Response *response)
{
    try
    {
        if (!mountPoint->HandleRequest(request, response))
            throw HttpException(HttpStatusCode::NotFound);
    }
    catch (HttpException &e)
    {
        if (!mountPoint->HandleException(request, response, &e))
        {
            response->SetStatusCode(e.GetStatus().GetStatusCode());
            response->SetStatusMessage(e.GetStatus().GetStatusMessage());
            response->GetBodyBuffer()->PutString(e.GetStatus().GetStatusMessage());
        }
    }
}

void VirtualHost::AddMountPoint(MountPoint *mountPoint) {
    m_rootMountPoint->AddNestedMount(mountPoint);
}

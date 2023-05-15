//
// Created by Alexis Lavaud on 05/04/2023.
//

#include "VirtualHost.h"
#include "ServerHost.h"

VirtualHost::VirtualHost()
{
}

VirtualHost::~VirtualHost()
{
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
    URL url(request->GetRawPath());
    MountPoint *mountPoint;
    size_t i;

    for (i = 0; i < m_mountPoints.size(); ++i)
    {
        mountPoint = m_mountPoints[i];

        if (mountPoint->Matches(request->GetRawPath()))
        {
            printf("Found a match : %p\n", mountPoint);
            break ;
        }
    }
    if (m_mountPoints.size() == i)
    {
        printf("404 or default route for %s\n", request->GetRawPath().c_str());
    }
    response->SendFile(url.GetAbsolutePath("./htdocs"));
}

const VirtualHost::tServerNameList &VirtualHost::GetServerNames() const
{
    return m_serverNames;
}

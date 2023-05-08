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

void VirtualHost::HandleRequest(Request *request, Response *response)
{
    URL url(request->GetRawPath());

    response->SendFile(url.GetAbsolutePath("./htdocs"));
}

const VirtualHost::tServerNameList &VirtualHost::GetServerNames() const
{
    return m_serverNames;
}

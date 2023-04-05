//
// Created by Alexis Lavaud on 05/04/2023.
//

#include "VirtualHost.h"

VirtualHost::VirtualHost()
{
}

VirtualHost::~VirtualHost()
{
}

#include "Http/Request.h"
#include "Http/Response.h"
#include "IO/DataBuffer.h"
#include <sstream>

void VirtualHost::HandleRequest(Request *request, Response *response)
{
    std::stringstream ss;

    ss << "<h1>Hello: " << reinterpret_cast<unsigned long>(this) << " !</h1>";
    response->GetOutputBuffer()->PutString(ss.str());
}

const VirtualHost::tServerNameList &VirtualHost::GetServerNames() const
{
    return m_serverNames;
}

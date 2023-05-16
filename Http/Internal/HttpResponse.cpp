//
// Created by Alexis Lavaud on 05/04/2023.
//

#include "HttpResponse.h"
#include "../HttpProtocolCodec.h"
#include "../HttpClientHandler.h"

HttpResponse::HttpResponse(HttpClientHandler *clientHandler)
{
    m_clientHandler = clientHandler;
    m_codec = clientHandler->GetProtocolCodec();
}

HttpResponse::~HttpResponse()
{
}


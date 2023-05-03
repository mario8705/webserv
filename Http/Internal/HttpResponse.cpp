//
// Created by Alexis Lavaud on 05/04/2023.
//

#include "HttpResponse.h"
#include "../HttpProtocolCodec.h"

HttpResponse::HttpResponse(HttpProtocolCodec *codec)
{
    m_codec = codec;
}

HttpResponse::~HttpResponse()
{
}

int HttpResponse::Write(const void *data, size_t n)
{
    return 0;
}

void HttpResponse::End()
{

}

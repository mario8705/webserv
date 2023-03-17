//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_PROTOCOLCODEC_H
#define WEBSERV_PROTOCOLCODEC_H
#include <string>
#include <map>
#include "HttpMethod.h"
#include "Request.h"
#include "Response.h"

class DataBuffer;
class IRequestHandler;

class ProtocolCodec
{
public:
    typedef std::map<std::string, std::string> tHttpHeaderMap;
    typedef std::map<std::string, HttpMethod> tHttpMethodsMap;

    ProtocolCodec(IRequestHandler *requestHandler, DataBuffer *inputBuffer, DataBuffer* outputBuffer);

    void ProcessData();

private:
    class HttpRequest : public Request
    {
    public:
        HttpRequest();
        ~HttpRequest();

        void SetMethod(HttpMethod method);
        void SetPath(std::string path);
    };

    class HttpResponse : public Response
    {
    public:
        HttpResponse();
        ~HttpResponse();
    };

    void DispatchRequest();
    void ParseRequest(std::string line);
    void ParseHeader(std::string line);
    static void EncodeResponse(DataBuffer *buffer, std::string httpVersion, int status, std::string message);
    static void EncodeHeaders(DataBuffer *buffer, const tHttpHeaderMap &headers);
    static void EncodeChunk(DataBuffer *buffer, DataBuffer *chunk);

    IRequestHandler *m_requestHandler;
    DataBuffer *m_inputBuffer;
    DataBuffer *m_outputBuffer;
    tHttpMethodsMap m_supportedMethods;
    int m_lineNumber;
    HttpMethod m_method;
    std::string m_path;
    std::string m_httpVersion;
    std::map<std::string, std::string> m_headers;
};


#endif //WEBSERV_PROTOCOLCODEC_H

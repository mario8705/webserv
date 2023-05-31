//
// Created by Alexis Lavaud on 23/03/2023.
//

#ifndef WEBSERV_HTTPPROTOCOLCODEC_H
#define WEBSERV_HTTPPROTOCOLCODEC_H
#include <string>
#include <map>
#include "HttpMethod.h"
#include "HttpVersion.h"

class HttpClientHandler;
class DataBuffer;
class Response;
class BufferEvent;
class IAsyncRequestHandler;

class HttpProtocolCodec
{
public:
    typedef std::map<std::string, std::string> tHeaderMap;

    explicit HttpProtocolCodec(HttpClientHandler *handler, DataBuffer *input, DataBuffer *output);
    ~HttpProtocolCodec();

    void ProcessDataInput();
    void OnOutputDrained();

    void WriteResponseHeader();
    void FinalizeResponse();

    /**
     * Writes data into the stream, with respect to the current transfer encoding.
     * @param data
     * @param n
     */
    void Write(const void *data, size_t n);
    void Write(DataBuffer *buffer);

    void AddHeader(const std::string &name, const std::string &value);

    DataBuffer *GetInputBuffer() const;
    DataBuffer *GetOutputBuffer() const;

private:
    void ParseRequestHeader(const std::string &line);
    void SetRequestHeader(const std::string &method, const std::string &rawPath, const std::string &httpVersion);
    void ParseHeader(const std::string &line);
    void WriteChunkHeader(size_t length);

    void DispatchRequest();

    HttpClientHandler *m_handler;
    DataBuffer *m_inputBuffer;
    DataBuffer *m_outputBuffer;
    BufferEvent *m_bufferEvent;
    tHeaderMap m_headers;
    bool m_requestHeaderParsed;
    HttpMethod m_method;
    HttpVersion m_httpVersion;
    std::string m_rawPath;
    IAsyncRequestHandler *m_asyncHandler;
    bool m_pendingFinalizeResponse;
    size_t m_bodyLength;

    bool m_chunked;
    int m_responseStatus;
    std::string m_responseMessage;
    tHeaderMap m_responseHeaders;
    bool m_headersSent;
};


#endif //WEBSERV_HTTPPROTOCOLCODEC_H

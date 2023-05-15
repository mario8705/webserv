//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_RESPONSE_H
#define WEBSERV_RESPONSE_H
#include <string>
#include <map>

class DataBuffer;
class HttpStatusCode;
class IAsyncRequestHandler;
class HttpProtocolCodec;
class HttpClientHandler;

enum HttpTransferEncoding
{
    kTransferEncoding_Chunked,
    kTransferEncoding_Stream,
};


class Response
{
public:
    typedef std::map<std::string, std::string> tHttpHeadersMap;

    virtual ~Response();

    void SetStatus(const HttpStatusCode &status);

    int GetStatusCode() const;
    void SetStatusCode(int status);

    std::string GetStatusMessage() const;
    void SetStatusMessage(std::string message);

    const tHttpHeadersMap &GetHeaders() const;
    tHttpHeadersMap &GetHeaders();
    void AddHeader(std::string key, std::string value);

    void SetAsyncHandler(IAsyncRequestHandler *asyncHandler);
    IAsyncRequestHandler *GetAsyncHandler() const;

    void SendFile(const std::string &path);

    HttpProtocolCodec *GetHttpCodec() const;

    void SetContentLength(size_t length);
    size_t GetContentLength() const;

    void SetChunked(bool chunked);
    bool IsChunked() const;

    virtual int Write(const void *data, size_t n) = 0;
    virtual void End() = 0;

protected:
    Response();

    HttpClientHandler *m_clientHandler;
    HttpProtocolCodec *m_codec;
    int m_status;
    std::string m_message;
    tHttpHeadersMap m_headers;
    IAsyncRequestHandler *m_asyncHandler;
    size_t m_contentLength;
    bool m_chunked;
};


#endif //WEBSERV_RESPONSE_H

//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_RESPONSE_H
#define WEBSERV_RESPONSE_H
#include <string>
#include <map>

class DataBuffer;
class HttpStatusCode;

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

    DataBuffer *GetOutputBuffer() const;

    /**
     * Detach the request from the server, to be handled by an external program (CGI).
     * When this flag is enabled, only the http response and some headers are written
     * @param external
     */
    void SetExternal(bool external);
    bool IsExternal() const;

protected:
    Response();

    int m_status;
    std::string m_message;
    tHttpHeadersMap m_headers;
    DataBuffer *m_outputBuffer;
    bool m_external;
};


#endif //WEBSERV_RESPONSE_H

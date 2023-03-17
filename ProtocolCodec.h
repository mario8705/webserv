//
// Created by Alexis Lavaud on 17/03/2023.
//

#ifndef WEBSERV_PROTOCOLCODEC_H
#define WEBSERV_PROTOCOLCODEC_H
#include <string>
#include <map>

class DataBuffer;

class ProtocolCodec
{
public:
    typedef std::map<std::string, std::string> tHttpHeaderMap;

    ProtocolCodec(DataBuffer *inputBuffer, DataBuffer* outputBuffer);

    void ProcessData();

private:
    void ParseRequest(std::string line);
    void ParseHeader(std::string line);
    static void EncodeResponse(DataBuffer *buffer, std::string httpVersion, int status, std::string message);
    void EncodeHeaders(const tHttpHeaderMap &headers);

    DataBuffer *m_inputBuffer;
    DataBuffer *m_outputBuffer;
    int m_lineNumber;
    std::string m_method;
    std::string m_path;
    std::string m_httpVersion;
    std::map<std::string, std::string> m_headers;
};


#endif //WEBSERV_PROTOCOLCODEC_H

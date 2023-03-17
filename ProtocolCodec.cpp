//
// Created by Alexis Lavaud on 17/03/2023.
//

#include "ProtocolCodec.h"
#include <sstream>
#include "DataBuffer.h"
#include "HttpException.h"

ProtocolCodec::ProtocolCodec(DataBuffer *inputBuffer, DataBuffer *outputBuffer)
    : m_inputBuffer(inputBuffer), m_outputBuffer(outputBuffer)
{
    m_lineNumber = 0;
}

void ProtocolCodec::ProcessData()
{
    std::string line;

    try {
        while (m_inputBuffer->Readln(line)) {
            if (m_lineNumber == 0) {
                ParseRequest(line);
            } else {
                if (line.empty()) {
                    DataBuffer buf;

                    printf("%s %s\n", m_method.c_str(), m_path.c_str());

                    EncodeResponse(&buf, m_httpVersion, 200, "Ok");
                    buf.PutString("Transfer-Encoding: chunked\r\n");
                    buf.PutString("\r\n");
                    buf.PutString("b\r\nOne Ok Rock\r\n");
                    buf.PutString("0\r\n\r\n");

                    m_lineNumber = 0;
                    m_headers.clear();

                    m_outputBuffer->AddBuffer(&buf);
                }
                else {
                    ParseHeader(line);
                }
            }
            ++m_lineNumber;
        }
        if (m_inputBuffer->GetLength() > 4)
            throw HttpException(404);
    }
    catch (const HttpException &e)
    {
        printf("Http exception (status = %d)\n", e.GetStatus());
    }
}

void ProtocolCodec::ParseRequest(std::string line)
{
    std::string method;
    std::string path;
    std::string httpVersion;
    size_t firstSep;
    size_t lastSep;

    firstSep = line.find_first_of(' ');
    lastSep = line.find_last_of(' ');
    if (firstSep == lastSep || firstSep == std::string::npos)
    {
        // TODO
    }
    method = line.substr(0, firstSep);
    path = line.substr(firstSep + 1, lastSep - firstSep - 1);
    httpVersion = line.substr(lastSep + 1);

    m_method = method;
    m_path = path;
    m_httpVersion = httpVersion;
}

static void ltrim(std::string &s)
{
    size_t i;

    for (i = 0; i < s.size(); ++i)
    {
        if (!std::isspace(s[i]))
            break ;
    }
    s.erase(0, i);
}

void ProtocolCodec::ParseHeader(std::string line)
{
    size_t sep;
    std::string key, value;

    sep = line.find_first_of(':');
    if (sep == std::string::npos)
    {
        /* TODO */
    }
    key = line.substr(0, sep);
    value = line.substr(sep + 1);

    ltrim(value);

    m_headers.insert(std::make_pair(key, value));
}

void ProtocolCodec::EncodeResponse(DataBuffer *buffer, std::string httpVersion, int status, std::string message)
{
    std::stringstream ss;

    ss << httpVersion << " " << status << " " << message << "\r\n";
    buffer->PutString(ss.str());
}

void ProtocolCodec::EncodeHeaders(const tHttpHeaderMap &headers)
{
    std::stringstream ss;
    tHttpHeaderMap::const_iterator it;

    for (it = headers.begin(); it != headers.end(); ++it)
    {
        ss << it->first << ": " << it->second << "\r\n";
    }
    m_outputBuffer->PutString(ss.str());
}
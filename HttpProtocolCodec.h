//
// Created by Alexis Lavaud on 23/03/2023.
//

#ifndef WEBSERV_HTTPPROTOCOLCODEC_H
#define WEBSERV_HTTPPROTOCOLCODEC_H
#include <string>
#include <map>

class HttpProtocolCodec
{
public:
    HttpProtocolCodec();
    ~HttpProtocolCodec();

    void ParseLine(const std::string &line);

private:

};


#endif //WEBSERV_HTTPPROTOCOLCODEC_H

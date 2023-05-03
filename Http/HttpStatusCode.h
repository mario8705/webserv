//
// Created by Alexis Lavaud on 05/04/2023.
//

#ifndef WEBSERV_HTTPSTATUSCODE_H
#define WEBSERV_HTTPSTATUSCODE_H
#include <string>

class HttpStatusCode
{
public:
    HttpStatusCode(int status, const char *message) throw();
    ~HttpStatusCode();

    int GetStatusCode() const;
    const char *GetStatusMessage() const;

    static HttpStatusCode Ok;
    static HttpStatusCode NotFound;

private:
    int m_status;
    const char *m_statusMessage;
};

#endif //WEBSERV_HTTPSTATUSCODE_H

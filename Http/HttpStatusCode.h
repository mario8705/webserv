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

    /* 2xx */
    static HttpStatusCode Ok;

    /* 4xx */
    static HttpStatusCode BadRequest;
    static HttpStatusCode NotFound;
    static HttpStatusCode MethodNotAllowed;
    static HttpStatusCode RequestURITooLong;

private:
    int m_status;
    const char *m_statusMessage;
};

#endif //WEBSERV_HTTPSTATUSCODE_H

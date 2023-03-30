//
// Created by alavaud on 3/14/23.
//

#ifndef WEBSERV_CONNECTIONHANDLER_H
#define WEBSERV_CONNECTIONHANDLER_H
#include <sys/socket.h>

class IConnectionHandler
{
public:
    virtual ~IConnectionHandler() {}

    virtual void HandleConnection(int fd, struct sockaddr *addr, socklen_t addrlen) = 0;
};

#endif //WEBSERV_CONNECTIONHANDLER_H

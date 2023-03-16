#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "CommandLineOpt.h"
#include "SelectEventLoop.h"
#include "ListenerEvent.h"
#include "ConnectionHandler.h"
#include "DataBuffer.h"
#include "FileEvent.h"
#include "ServerHost.h"

int main(int argc, char *argv[])
{
    SelectEventLoop eventLoop;
    std::vector<ListenerEvent *> listeners;
    ListenerEvent *serv;

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    ServerHost *serverHost = new ServerHost(&eventLoop);

    for (int i = 0; i < 10; ++i)
    {
        sin.sin_port = htons(8080 + i);

        serv = ListenerEvent::CreateAndBind(serverHost, (struct sockaddr *) &sin, sizeof(sin), 10);
        eventLoop.RegisterEvent(serv);

        listeners.push_back(serv);
    }

    eventLoop.Run();
    return 0;
}
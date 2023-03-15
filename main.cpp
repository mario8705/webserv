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

class MyHandler : public IConnectionHandler
{
public:
    void HandleConnection(int fd, struct sockaddr *addr, socklen_t addrlen)
    {
        printf("New connection :D\n");
    }
};

int main(int argc, char *argv[])
{
    CommandLineOpt opt;
    SelectEventLoop eventLoop;

    opt.Parse(argc, argv);

    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_port = htons(8080);
    sin.sin_addr.s_addr = INADDR_ANY;

    ListenerEvent *serv = ListenerEvent::CreateAndBind(new MyHandler, (struct sockaddr *) &sin, sizeof(sin), 10);
    eventLoop.RegisterEvent(serv);

    eventLoop.Run();
    return 0;
}
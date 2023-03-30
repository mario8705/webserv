#include <signal.h>
#include "Webserv.h"

int main(int argc, char *argv[])
{
    Webserv webserv;

    signal(SIGPIPE, SIG_IGN);
    webserv.CreateServer(NetworkAddress4(8080));
    webserv.Run();
    return 0;
}
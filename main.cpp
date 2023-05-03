#include <csignal>
#include "Webserv.h"

static Webserv *s_instance = NULL;

static void handler_stub(int)
{
    s_instance->Stop();
}

int main(int argc, char *argv[])
{
    Webserv webserv;

    s_instance = &webserv;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, handler_stub);
    webserv.CreateServer(NetworkAddress4(8080));
    webserv.Run();
    return 0;
}
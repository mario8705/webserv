#include <csignal>
#include "Webserv.h"

static Webserv *s_instance = NULL;

static void handler_stub(int)
{
    s_instance->Stop();
}

#include "Http/URL.h"

int main(int argc, char *argv[])
{
    Webserv webserv;

    /*
    URL::Decode("Hello%20World%");
    URL::Decode("Hello%20World%2");
    URL::ParseURL("/test/resource/123%20 Hello-lol?raw=true&test=bonjour");
    return 0;
     */

    s_instance = &webserv;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, handler_stub);
    webserv.CreateServer(NetworkAddress4(8080));
    webserv.Run();
    return 0;
}
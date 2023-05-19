#include <csignal>
#include "Webserv.h"
#include <iostream>

static void handler_stub(int)
{
    Webserv::GetInstance()->Stop();
}

int main(int argc, char *argv[])
{
    Webserv webserv;
    std::string configPath = "webserv.conf";

    if (argc >= 3)
    {
        std::cerr << "Too many argument, usage: ./webserv [config]" << std::endl;
        return 1;
    }
    if (2 == argc)
        configPath = argv[1];
    if (!webserv.LoadConfig(configPath))
    {
        std::cerr << "Could not load configuration" << std::endl;
        return 1;
    }

    if (!webserv.Bind())
        return 1;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, handler_stub);
    webserv.Run();
    return 0;
}
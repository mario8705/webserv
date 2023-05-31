#include <csignal>
#include <iostream>
#include <sys/wait.h>
#include "Webserv.h"

static void handler_stub(int)
{
    Webserv::GetInstance()->Stop();
}

static void collect_child_status(int)
{
    pid_t p;
    int status;

    while ((p = waitpid(-1, &status, WNOHANG)) > 0)
    {
        std::cout << "Child died: " << p << std::endl;
    }
}

#include "Cgi/CgiManager.hpp"

int main(int argc, char *argv[])
{
    Webserv webserv;
    std::string configPath = "webserv.conf";

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, collect_child_status);
    signal(SIGTERM, handler_stub);

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

    webserv.Run();
    return 0;
}
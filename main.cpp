#include <cstdlib>
#include <cstdio>
#include "Webserv.h"

int main(int argc, char *argv[])
{
    Webserv webserv;

    webserv.CreateServer(NetworkAddress4(8080));
    webserv.Run();
    return 0;
}
#include <csignal>
#include "Webserv.h"
#include <iostream>

static void handler_stub(int)
{
    Webserv::GetInstance()->Stop();
}

std::string subsitute_vars(const std::string &input, const std::map<std::string, std::string> &vars)
{
    size_t pos;
    size_t last;
    std::string ret;
    std::map<std::string, std::string>::const_iterator it;

    for (pos = 0; pos < input.size(); )
    {
        if ('$' == input[pos] && (pos == 0 || input[pos - 1] != '\\'))
        {
            last = ++pos;
            while (last < input.size() && (input[last] == '_' || std::islower(input[last])))
                ++last;
            it = vars.find(input.substr(pos, last - pos));
            if (it != vars.end()) {
                ret.insert(ret.end(), it->second.begin(), it->second.end());
                pos = last;
            }
            else
            {
                ret.insert(ret.end(), '$');
            }
        }
        else
        {
            last = pos + 1;
            while (last < input.size() && '$' != input[last])
                ++last;
            ret.insert(ret.end(), input.begin() + pos, input.begin() + last);
            pos = last;
        }
    }
    return ret;
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
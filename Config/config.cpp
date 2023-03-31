#include "config.hpp"

Config::Config()
{
    std::cout << "config on\n";
}

Server::Server(std::vector<std::string> content) : _serverconfig(content)
{
    std::cout << "server creation\n";
    /*for (size_t i = 0; i < _servconfig.size(); i++)
    {
        std::cout << _servconfig[i];
    }*/
}

Config::~Config() {
    for (size_t i = 0; i < _servconfig.size(); i++) {
        delete _servconfig[i];
    }
    std::cout << "config destructor\n";
}

Server::~Server() {
    std::cout << "server destructor\n";
}

std::vector<std::string> Config::getConfig() {return(_config);}
std::vector<std::string> Server::getServConfig() {return(_serverconfig);}
std::string Server::getServConfigIndex(size_t index) {return(_serverconfig[index]);}

std::vector<Server *> Config::getServConfig() {return(_servconfig);}

std::vector<std::string>	split_with_space(std::string line)
{
    std::vector<std::string>	words;

    if (line.size() == 0)
        return (words);
    int k = 0;
    int j = -1;
    for (int i = 0; i < line.size(); i++)
    {
        if (line[i] == ' ' || line[i] == '\t')
        {
            if (i != 0)
                k++;
            while (line[i] == ' ' || line[i] == '\t')
                i++;
            i--;
        }
        else
        {
            if (j != k)
            {
                j = k;
                words.push_back("");
            }
            words[k].push_back(line[i]);
        }
    }
    return (words);
}

std::vector<std::string> Config::tokenization(std::string line, std::vector<std::string> &conf)
{
    std::vector<std::string> clean;
    std::vector<std::string> tmp;

    size_t is_title = line.find('{');
    if (is_title != std::string::npos)
    {
        std::size_t first_non_space = line.find_first_not_of(" /t/n");
        std::string title_line = line.substr(first_non_space, line.length() - 1 - first_non_space);
        conf.push_back("Title : " + title_line);
        conf.push_back("Open bracket : {");
        conf.push_back("End of line \n");
        return(conf);
    }
    tmp = split_with_space(line);
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (tmp[i] == "{")
            conf.push_back("Open bracket : " + tmp[i]);
        else if (tmp[i] == "}")
            conf.push_back("Close bracket : " + tmp[i]);
        else
            conf.push_back("Text : " + tmp[i]);
    }
    conf.push_back("End of line \n");
    return(conf);
}

void Config::makeConfig()
{
    std::string filename = "../config.conf";
    std::ifstream file(filename.c_str());
    std::vector<std::string> final;

    if (file.good())
    {
        int it;
        std::string line;
        while (std::getline(file, line, '\n')) {
            if (line.size() != 0)
                Config::tokenization(line, final);
        }
        file.close();
    }
    else
        exit(1);
    final.push_back("End of file\n");
    _config = final;
}

void Config::split_serv(std::vector<std::string> args)
{
    std::vector<std::string> final;
    size_t i = 0;

    while (i < args.size())
    {
        size_t title = args[i].find("Title : server");
        if (title != std::string::npos)
        {
            final.push_back(args[i]);
            i++;
            size_t split = args[i].find("Title : server");
            while (i < args.size())
            {
                split = args[i].find("Title : server");
                if (split != std::string::npos)
                    break;
                final.push_back(args[i]);
                i++;
            }
            //_servconfig.push_back(final);
            Server* new_server = new Server(final);
            _servconfig.push_back(new_server);
            //std::cout << "End of Server\n\n";
            final.clear();
        }
        else
            i++;
    }
    //for (size_t k = 0; k < _test[0].)
}

int main()
{
    std::vector<std::string> infile;
    std::vector<std::string> final;
    std::vector<Server*> servers;
    Config config;
    config.makeConfig();
    final = config.getConfig();
    config.split_serv(final);
    servers = config.getServConfig();
    for (size_t i = 0; i < servers.size(); i++)
{
        for (size_t j = 0; j < servers[i]->getServConfig().size(); j++)
        {
            std::cout << servers[i]->getServConfigIndex(j);
        }
        std::cout << "END SERV\n\n";
}
    //config conf(infile);
}
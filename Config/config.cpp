#include "config.hpp"

config::config(std::string content) : _stringconfig(content)
{
    std::cout << "config on\n";
}

server::server(std::string content) : _stringconfig(content)
{
    std::cout << "server creation\n";
}

config::~config() {
    std::cout << "config destructor\n";
}

server::~server() {
    //std::cout << "server destructor\n";
}

std::string config::getStringConfig() {return(_stringconfig);}
std::string server::getStringConfig() {return(_stringconfig);}


std::vector<std::string> config::makeServ(std::string conf)
{
    std::vector<std::string> servers;
    size_t pos = conf.find("server {");
    size_t pos_next;
    while (pos != std::string::npos)
    {
        pos_next = conf.find("server {", pos + 1);
        if (pos_next == std::string::npos)
        {
            servers.push_back(conf.substr(pos));
        }
        else
        {
            servers.push_back(conf.substr(pos, pos_next - pos));
        }
        pos = pos_next;
    }
    return servers;
}
/*
    Récupère le fichier de config (config.conf), vérifie s'il existe bien.
    Renvois ledit document sous forme de string.
 */
std::string getconfigfile(void)
{
    std::string filename = "../config.conf";
    std::ifstream file(filename.c_str());
    std::string content;

    if (file.good())
    {
        int it;
        std::string line;
        while (std::getline(file, line, '\n')) {
            content += line;
        }

        file.close();
    }
    else
        throw(config::InvalidConfigFile());
    return(content);
}

/*std::map <std::string, std::string> config::configtomapstring(std::string conf)
{
    std::stringstream ss(conf);

}*/


int main()
{
    try{
        std::vector<std::string> splited_serv;
        std::string tmp_conf = getconfigfile();
        config conf(tmp_conf);
        splited_serv = conf.makeServ(tmp_conf);
        std::vector<server> servers;
        for (size_t i = 0; i < splited_serv.size(); i++)
        {
            servers.push_back(splited_serv[i]);
        }
          std::cout << servers[0].getStringConfig() << std::endl;
          std::cout << servers[1].getStringConfig() << std::endl;
          std::cout << servers[2].getStringConfig() << std::endl;


    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return(0);
}
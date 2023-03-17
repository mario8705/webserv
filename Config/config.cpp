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

/*
 *
 * Découpe le fichier de configuration à chaue "server {" afin de
 * rendre plus simple la lecture du fichier de configuration
 *
 * */
std::vector<std::string> config::makeServ(std::string conf)
{
    std::vector<std::string> servers;
    size_t pos = conf.find("server {");
    size_t pos_next;
    while (pos != std::string::npos)
    {
        pos_next = conf.find("server {", pos + 1);
        if (pos_next == std::string::npos)
            servers.push_back(conf.substr(pos));
        else
            servers.push_back(conf.substr(pos, pos_next - pos));
        pos = pos_next;
    }
    return servers;
}
/*
 *
 * Récupère le fichier de config (config.conf), vérifie s'il existe bien.
 * Renvois ledit document sous forme de string.
 *
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
            content += '\n';
        }

        file.close();
    }
    else
        throw(config::InvalidConfigFile());
    return(content);
}

std::vector<std::string> server::splitted_lines(){
    //std::cout << _stringconfig <<std::endl;
    std::vector<std::string> splitted_conf;
    std::vector<std::string> final_conf;
    std::vector<std::string> result;
    std::map<std::string, std::string> map;
    std::string buffer;
    std::stringstream ss(_stringconfig);
    while (std::getline(ss, buffer, '\n'))
    {
        splitted_conf.push_back(buffer);
    }
    for (size_t i = 0; i < splitted_conf.size(); i++)
    {
        std::size_t first_non_space = splitted_conf[i].find_first_not_of(' ');
        final_conf.push_back(splitted_conf[i].substr(first_non_space));
        //std::cout << final_conf[i] << std::endl;
    }
    for (size_t i = 0; i < final_conf.size(); i++)
    {
        size_t pos = final_conf[i].find(' ');
        if (pos != std::string::npos)
        {
            result.push_back(final_conf[i].substr(0, pos));
            result.push_back(final_conf[i].substr(pos + 1));
        }
        else
        {
            size_t check = final_conf[i].find("}");
            if (check != std::string::npos)
                result.push_back(final_conf[i]);
            else
            {
                std::cout << "Wrong syntax in Conf.config\n";
                exit(1);
            }
        }
    }
    std::cout << std::endl;
    for (size_t i = 2; i < result.size(); i++)
    {
        map.insert(std::make_pair(result[i], result[i + 1]));
        i++;
    }

    for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); it++) {
        std::cout << it->first << " : " << it->second << std::endl;
    }
    return (result);
}

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
            servers[i].splitted_lines();
            //std::cout << servers[i].getStringConfig() << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return(0);
}
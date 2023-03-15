#include "config.hpp"

config::config(std::string content) : _stringconfig(content)
{
    std::cout << "Gameson\n";
    std::cout << getStringConfig() << std::endl;
}

config::~config() {
    std::cout << "destructor\n";
}

std::string config::getStringConfig() {return(_stringconfig);}
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
        std::string line;
        while (std::getline(file, line)) {
            content += line;
            content += '\n';
        }
        file.close();
    }
    else
        throw(config::InvalidConfigFile());
    return(content);
}

int main()
{
    try{
        std::string tmp_conf = getconfigfile();
        config conf(tmp_conf);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return(0);
}
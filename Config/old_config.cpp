#include "config.hpp"

config::config(std::string content) : _stringconfig(content)
{
    std::cout << "Gameson\n";
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
        int it;
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

std::map <std::string, std::string> config::configtomapstring(std::string conf)
{
    std::stringstream ss(conf);

    while (getline())

}

/*std::map <std::string, std::string> config::configtomap(std::vector<std::string> conf)
{
    std::map<std::string, std::string> my_map;
    //for (size_t i = 3; i < conf.size(); i++)
    //{
    my_map.insert(std::make_pair(conf[2], conf[3] + conf[4]));
    /*for (size_t i = 2; i < conf.size(); i++)
    {
        for (size_t j = 0; j < conf.size(); j++)
        {
            //my_map.insert(conf[2])
        }
    }*/

//}
for (std::map<std::string, std::string>::iterator it = my_map.begin(); it != my_map.end(); it++) {
std::cout << it->first << " : " << it->second << std::endl;
}
//return(my_map);
}*/

std::vector<std::string> tokenize(std::string const& str)
{
    std::vector<std::string> tokens;
    std::vector<std::string> final_tokens;
    std::stringstream ss(str); // Créer un stringstream à partir de la chaîne de caractères

    std::string token;
    while (std::getline(ss, token, ' ')) // Parcourir chaque mot séparé par un espace
    {
        tokens.push_back(token); // Ajouter le mot au vecteur de tokens
    }
    for (size_t i = 0; i < tokens.size(); i++)
    {
        size_t brace_open_pos = tokens[i].find("{");
        size_t brace_close_pos = tokens[i].find("}");
        if (tokens[i].size() > 1 ||
            (brace_open_pos != std::string::npos || brace_close_pos != std::string::npos))
        {
            final_tokens.push_back(tokens[i]);
        }
    }
    return final_tokens;
}

int main()
{
    try{
        std::string tmp_conf = getconfigfile();
        config conf(tmp_conf);
        std::vector<std::string> tokens_vector = tokenize(tmp_conf);
        for (size_t i = 0; i < tokens_vector.size(); i++)
        {
            std::cout << tokens_vector[i] << std::endl;
        }
        //conf.configtomap(tokens_vector);
        conf.configtomapstring(tmp_conf);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return(0);
}
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

std::vector<std::string> tokenize(std::string const& str)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str); // Créer un stringstream à partir de la chaîne de caractères

    std::string token;
    while (std::getline(ss, token, ' ')) // Parcourir chaque mot séparé par un espace
    {
        if (token.size() != 0 )
            tokens.push_back(token); // Ajouter le mot au vecteur de tokens
    }
    return tokens;
}

int main()
{
    try{
        std::string tmp_conf = getconfigfile();
        config conf(tmp_conf);
        std::vector<std::string> tokens_vector = tokenize(tmp_conf);
        for (size_t i = 0; i < tokens_vector.size(); i++)
        {
            //if (tokens_vector[i].find("\n") != std::string::npos && tokens_vector[i + 1].find("\n") != std::string::npos)
                std::cout << tokens_vector[i] << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return(0);
}
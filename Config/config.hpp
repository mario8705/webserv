#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cctype>

class Server {
public:
    Server(std::vector<std::string> content);
    ~Server();
    std::vector<std::string> getServConfig();
    std::string getServConfigIndex(size_t index);

private:
    std::vector<std::string> _serverconfig;
};

class Config {

public :
    //config(std::vector<std::string> conf);
    Config();
    ~Config();
    std::vector<std::string> getConfig();
    std::vector<Server> getServConfig();

    /*Tokenization process make _config*/
    void makeConfig();
    std::vector<std::string> tokenization(std::string line, std::vector<std::string> &conf);
    void split_serv(std::vector<std::string> args);

    class InvalidConfigFile : public std::exception {
    public :
        virtual const char* what() const throw() {return "invalid configfile";}
    };

private:
    std::vector<std::string> _config;
    std::vector<Server> _servconfig;
};


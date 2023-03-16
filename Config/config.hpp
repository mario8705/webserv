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


class config {

public :
    config(std::string content);
    ~config();
    //void &operator=(const config &new);
    std::vector<std::string> makeServ(std::string conf);
    std::map<std::string, std::string> configtomapstring(std::string conf);

    std::string getStringConfig();

    class InvalidConfigFile : public std::exception {
        public :
            virtual const char* what() const throw() {return "invalid configfile";}
    };

private:
    std::string _stringconfig;
    std::map<std::string, std::string> _mapconfig;
};

class server {
public:
    server(std::string content);
    ~server();
    std::string getStringConfig();

private:
    std::string _stringconfig;
};
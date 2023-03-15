#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>


class config {

public :
    config(std::string content);
    ~config();
    //void &operator=(const config &new);
    std::map<std::string, std::string> configtomap(std::string config);
    std::string getStringConfig();

    class InvalidConfigFile : public std::exception {
        public :
            virtual const char* what() const throw() {return "invalid configfile";}
    };

private:
    config();
    std::string _stringconfig;
    std::map<std::string, std::string> _mapconfig;



};
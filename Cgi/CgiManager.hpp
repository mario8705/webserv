//
// Created by tgriffit on 4/3/23.
//

#ifndef WEBSERV_CGIMANAGER_HPP
#define WEBSERV_CGIMANAGER_HPP

#include <iostream>
#include <unistd.h>
#include <map>

#define CGI_PATH "cgi-bin/"

class CgiManager {

    const std::string knownCgi[3] = {"python-cgi.py", "php-cgi", "cpp-cgi"};
    std::string cgiResponse;
    std::map<std::string, std::string> serVarMap;

public:
    CgiManager();
    CgiManager(std::string cgiToExecute, char **env,
               std::map<std::string, std::string> &ServerVariablesMap);
    CgiManager(const CgiManager &copy);
    ~CgiManager();
    CgiManager &operator=(const CgiManager &toAssign);

    void execute(std::string cgiName, char **env);
    void sendEnvVarsToCgi();
};


#endif //WEBSERV_CGIMANAGER_HPP

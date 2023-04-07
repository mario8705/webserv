//
// Created by tgriffit on 4/3/23.
//

#ifndef WEBSERV_CGIMANAGER_HPP
#define WEBSERV_CGIMANAGER_HPP

#include <iostream>
#include <unistd.h>
#include <map>

#define CGI_PATH "cgi-bin/"
#define PHP_CMD "/bin/php"

class CgiManager {

    //const std::string knownCgi[3] = {"python-cgi.py", "php-cgi.php", "cpp-cgi"};
    std::string cgiResponse;
    std::map<std::string, std::string> serVarMap;

	void convertCgiFileToCgiResponse(const std::string &cgiFileName);

public:

	CgiManager();
	CgiManager(std::string cgiToExecute, std::map<std::string, std::string> &ServerVariablesMap);
	CgiManager(const CgiManager &copy);
	~CgiManager();
	CgiManager &operator=(const CgiManager &toAssign);
    void execute(const std::string &cgiName);

	char ** convertEnvMap();
	const std::string &getCgiResponse() const;
};


#endif //WEBSERV_CGIMANAGER_HPP

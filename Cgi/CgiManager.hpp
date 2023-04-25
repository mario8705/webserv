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
#define BASH_CMD "/bin/bash"

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

    class CgiException: public std::exception
    {

        virtual const char *what() const throw() = 0;

    public:
        class ForkException: public std::exception
        {
        public:
            virtual const char *what() const throw();
        };
        class PipeException: public std::exception
        {
        public:
            virtual const char *what() const throw();
        };
        class ExecException: public std::exception
        {
        public:
            virtual const char *what() const throw();
        };
        class Dup2Exception: public std::exception
        {
        public:
            virtual const char *what() const throw();
        };
        class OpenException: public std::exception
        {
        public:
            virtual const char *what() const throw();
        };
        class BadFormatException: public std::exception
        {
        public:
            virtual const char *what() const throw();
        };

    };
};


#endif //WEBSERV_CGIMANAGER_HPP

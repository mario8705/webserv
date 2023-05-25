//
// Created by tgriffit on 4/3/23.
//

#ifndef WEBSERV_CGIMANAGER_HPP
#define WEBSERV_CGIMANAGER_HPP
#include <unistd.h>
#include <iostream>
#include <map>
#include <vector>

class CgiManager
{
public:
    typedef std::map<std::string, std::string> tEnvMap;

	CgiManager(const std::string &pathToCgi, const tEnvMap &envMap);
	~CgiManager();

    pid_t SpawnSubProcess();

	int GetMISO() const;
    int GetMOSI() const;

    class CgiException: public std::exception
    {
    public:
        explicit CgiException(const char *msg) throw();
        ~CgiException() throw();

        virtual const char *what() const throw();

    private:
        const char *m_message;
    };

private:
    std::string m_path;
    std::vector<std::string> m_envList;
    int m_miso;
    int m_mosi;

    CgiManager(const CgiManager &copy);
    CgiManager &operator=(const CgiManager &toAssign);

    static int SpawnSubProcess(const std::string &path, const std::vector<std::string> &args, const std::vector<std::string> &envs);
    static std::vector<std::string> SerializeEnvMap(const tEnvMap &envMap);
};


#endif //WEBSERV_CGIMANAGER_HPP

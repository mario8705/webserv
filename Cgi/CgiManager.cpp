//
// Created by tgriffit on 4/3/23.
//

#include <fstream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>
#include <vector>
#include "CgiManager.hpp"

int executeProcess(const std::string &path, const std::vector<std::string> &args, const std::vector<std::string> &envs);

CgiManager::CgiManager() {

}

CgiManager::CgiManager(std::string cgiToExecute, std::map<std::string, std::string> &ServerVariablesMap)
                       : serVarMap(ServerVariablesMap) {
    execute(cgiToExecute);
}

CgiManager::CgiManager(const CgiManager &copy) {
    *this = copy;
}

CgiManager::~CgiManager() {

}

CgiManager &CgiManager::operator=(const CgiManager &toAssign) {
    this->cgiFd = toAssign.cgiFd;
    this->serVarMap = toAssign.serVarMap;
    return *this;
}

class AutoCloseFileDescriptor {
public:
    AutoCloseFileDescriptor()
    {
        m_fd = -1;
    }

    AutoCloseFileDescriptor(int fd)
        : m_fd(fd)
    {
    }

    ~AutoCloseFileDescriptor()
    {
        Replace(-1);
    }

    void Replace(int fd)
    {
        if (m_fd >= 0)
            close(m_fd);
        m_fd = fd;
    }

    int GetFd() const
    {
        return m_fd;
    }

private:
    int m_fd;
};

/**
 * Si le CGI s'execute correctement, sa sortie standard est envoyee dans la string CgiManager->cgiResponse.
 * @param cgiName = le Cgi doit se finir par .sh ou .php
 * @throw CgiException::OpenException, CgiException::Dup2Exception, CgiException::ExecException
 */
void CgiManager::execute(const std::string &cgiName) { //todo: modifier le env original
	int fds_in[2], fds_out[2];
    AutoCloseFileDescriptor cgiFdManaged;
    int cgiFd = open((CGI_PATH + cgiName).c_str(), O_RDWR);
    if (cgiFd < 0)
        throw CgiException::OpenException();
    cgiFdManaged.Replace(cgiFd);

    if (pipe(fds_out) < 0)
        throw CgiException::PipeException();

    int pid = fork();
    if (pid == -1)
    {

        throw CgiException::ForkException();
    }
    if (pid == 0)
    {
        close(fds_out[0]);

        if (dup2(cgiFd, STDIN_FILENO) == -1)
            throw CgiException::Dup2Exception();
        close(cgiFd);

        if (dup2(fds_out[1], STDOUT_FILENO) == -1)
            throw CgiException::Dup2Exception();
        close(fds_out[1]);
        std::string executor;
        if (cgiName.rfind(".php") != std::string::npos)
            executor = PHP_CMD;
        else if (cgiName.rfind(".sh") != std::string::npos)
            executor = BASH_CMD;
        else
            throw CgiException::BadFormatException();

        std::string command = " " CGI_PATH + cgiName;

        std::vector<std::string> env = convertEnvMap();
        std::vector<std::string> args;
        args.push_back(command);

        executeProcess(command, args, env);
        throw CgiException::ExecException();
    }
    else
    {
        close(fds_out[1]);
        close(cgiFd);
    }
    while (waitpid(pid, NULL, 0) != -1);
    cgiFd = fds_out[0];

}

std::vector<std::string> CgiManager::convertEnvMap() {
    {
        { //todo: remove quand j'aurais la vrai map post-parsing/recuperation de la methode par le serveur
            serVarMap["REQUEST_METHOD"] = "[PH]Rqst Meth"; //contient la méthode HTTP utilisée pour effectuer la requête (par exemple, GET, POST, etc.)
            serVarMap["QUERY_STRING"] = "[PH]QUERY STR";    // contient la chaîne de requête de l'URL de la requête HTTP
            serVarMap["CONTENT_TYPE"] = "[PH]text/html";    // contient le type de contenu de la requête HTTP (par exemple, application/json, multipart/form-data, etc.)
            serVarMap["CONTENT_LENGTH"] = "42";         // contient la longueur du contenu de la requête HTTP
        }
    }

    std::vector<std::string> env;
    std::map<std::string, std::string>::iterator itMap = serVarMap.begin();
    int i = 0;
    for (; itMap != serVarMap.end() ; ++itMap, ++i) {
        std::string key = itMap->first;
        env.push_back(key + itMap->second);
    }
    return env;
}

int executeProcess(const std::string &path, const std::vector<std::string> &args, const std::vector<std::string> &envs)
{
    int n;

    /* Creer ton c array de env et arg */
    const char *envTab[envs.size() + 1];
    int i = 0;
    for (; i < envs.size(); ++i) {
        envTab[i] = envs[i].c_str();
    }
    envTab[i] = NULL;

    const char *argsTab[args.size() + 1];
    i = 0;
    for (;i < args.size(); ++i) {
        argsTab[i] = args.at(i).c_str();
    }
    argsTab[i] = NULL;
    n = execve(path.c_str(), const_cast<char * const *>(argsTab), const_cast<char * const *>(envTab));



    return n;
}


int CgiManager::getCgiFd() const
{
	return cgiFd;
}

const char *CgiManager::CgiException::ForkException::what() const throw() {
    return "Fork failed\n";
}

const char *CgiManager::CgiException::PipeException::what() const throw() {
    return "Pipe failed\n";
}

const char *CgiManager::CgiException::ExecException::what() const throw() {
    return "Execve failed\n";
}

const char *CgiManager::CgiException::Dup2Exception::what() const throw() {
    return "Dup2 failed\n";
}

const char *CgiManager::CgiException::OpenException::what() const throw() {
    return "File to open not found\n";
}

const char *CgiManager::CgiException::BadFormatException::what() const throw() {
    return "The CGI isn't a .sh either a .php script\n";
}

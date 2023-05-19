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

CgiManager::CgiManager(std::string pathToCgi, std::map<std::string, std::string> &ServerVariablesMap)
                       : serVarMap(ServerVariablesMap) {
    execute(pathToCgi);
}

CgiManager::CgiManager(const CgiManager &copy) {
    *this = copy;
}

CgiManager::~CgiManager() {

}

CgiManager &CgiManager::operator=(const CgiManager &toAssign) {
    this->cgiFdIn = toAssign.cgiFdIn;
    this->cgiFdOut = toAssign.cgiFdOut;
    this->cgiPid = toAssign.cgiPid;
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
 * @param cgiPath = le Cgi doit se finir par .sh ou .php
 * @throw CgiException::OpenException, CgiException::Dup2Exception, CgiException::ExecException
 * @return A malloc'd int: cgi's PID, pipe_webserv_to_cgi, pipe_cgi_to_webserv
 */
void CgiManager::execute(const std::string &cgiPath) {
	int fds_in[2], fds_out[2];
    AutoCloseFileDescriptor cgiFdManaged;
    int cgiFd = open(cgiPath.c_str(), O_RDWR);
    if (cgiFd < 0)
        throw CgiException::OpenException();
    cgiFdManaged.Replace(cgiFd);

    if (pipe(fds_out) < 0 || pipe(fds_in) < 0)
        throw CgiException::PipeException();

    int pid = fork();
    if (pid == -1)
        throw CgiException::ForkException();
    else if (pid == 0)
    {
        dup2(fds_in[FD_READ], fds_out[FD_READ]);
        close(fds_in[FD_READ]);
        close(fds_out[FD_WRITE]);

        if (dup2(cgiFd, fds_in[FD_WRITE]) == -1)
            throw CgiException::Dup2Exception();
        close(cgiFd);

        if (dup2(fds_in[FD_WRITE], fds_out[FD_READ]) == -1)
            throw CgiException::Dup2Exception();
        close(fds_in[FD_WRITE]);

        std::vector<std::string> env = convertEnvMap();
        std::vector<std::string> args;
        args.push_back(cgiPath);

        executeProcess(cgiPath, args, env);
        throw CgiException::ExecException();
    }
    else
    {
        close(fds_in[FD_READ]);
        close(fds_out[FD_WRITE]);
        close(cgiFd);
    }
    //while (waitpid(pid, NULL, 0) != -1);
    cgiPid = pid;
    cgiFdIn = fds_in[FD_WRITE];
    cgiFdOut = fds_out[FD_READ];
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


int CgiManager::getCgiFdIn() const
{
	return cgiFdIn;
}

int CgiManager::getCgiFdOut() const {
    return cgiFdOut;
}

int CgiManager::getCgiPid() const {
    return cgiPid;
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

//
// Created by tgriffit on 4/3/23.
//

#include <fstream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>
#include "CgiManager.hpp"

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
    this->cgiResponse = toAssign.cgiResponse;
    this->serVarMap = toAssign.serVarMap;
    return *this;
}

#
/**
 * Si le CGI s'execute correctement, sa sortie standard est envoyee dans la string CgiManager->cgiResponse.
 * @param cgiName = le Cgi doit se finir par .sh ou .php
 * @throw CgiException::OpenException, CgiException::Dup2Exception, CgiException::ExecException
 */
void CgiManager::execute(const std::string &cgiName) { //todo: modifier le env original

    convertEnvMap();
    char **env = convertEnvMap();
	int fds_in[2], fds_out[2];
	//const std::string cgiFileName("cgiFile.txt");
    int cgiFd = open((CGI_PATH + cgiName).c_str(), O_RDWR);
    if (cgiFd < 0)
        throw CgiException::OpenException();

    pipe(fds_out);

    int pid = fork();
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
        char *arg[] = { const_cast<char *>(command.c_str()), NULL};
        execve(executor.c_str(), arg, env);
        throw CgiException::ExecException();
    }
    else
    {
        close(fds_out[1]);
        close(cgiFd);
        ssize_t n;
        char buffer[BUFSIZ];
        while ((n = read(fds_out[0], buffer, sizeof(buffer))) > 0) {
            write(1, buffer, n);
            cgiResponse += buffer;
        }
        close(fds_out[0]);
    }
    while (waitpid(pid, NULL, 0) != -1);
    //convertCgiFileToCgiResponse(cgiFileName);

    for (size_t i = 0; env[i]; i++)
        delete[] env[i];
    delete[] env;
}

char ** CgiManager::convertEnvMap() {
    {
        { //todo: remove quand j'aurais la vrai map post-parsing/recuperation de la methode par le serveur
            serVarMap["REQUEST_METHOD="] = "[PH]Rqst Meth"; //contient la méthode HTTP utilisée pour effectuer la requête (par exemple, GET, POST, etc.)
            serVarMap["QUERY_STRING"] = "[PH]QUERY STR";    // contient la chaîne de requête de l'URL de la requête HTTP
            serVarMap["CONTENT_TYPE"] = "[PH]text/html";    // contient le type de contenu de la requête HTTP (par exemple, application/json, multipart/form-data, etc.)
            serVarMap["CONTENT_LENGTH"] = "42";         // contient la longueur du contenu de la requête HTTP
        }
    }

    char **env = new char *[serVarMap.size()] ;
    std::map<std::string, std::string>::iterator itMap = serVarMap.begin();
    int i = 0;
    for (; itMap != serVarMap.end() ; ++itMap, ++i) {
        env[i] = new char[itMap->second.length() + 1];
        std::strcpy(env[i], itMap->second.c_str());
    }
    env[i] = NULL;
    return env;
}

void CgiManager::convertCgiFileToCgiResponse(const std::string &cgiFileName)
{
	std::ifstream cgiFile(cgiFileName.c_str());
	char buffer[BUFSIZ];

	if (cgiFile.is_open())
	{
		while (cgiFile.getline(buffer, BUFSIZ))
		{
			cgiResponse += buffer;
			cgiResponse += '\n';
		}
		cgiFile.close();
	}
}

const std::string &CgiManager::getCgiResponse() const
{
	return cgiResponse;
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

//
// Created by tgriffit on 4/3/23.
//

#include <fstream>
#include <cstdlib>
#include "CgiManager.hpp"

CgiManager::CgiManager() {

}

CgiManager::CgiManager(std::string cgiToExecute, char **env,
                       std::map<std::string, std::string> &ServerVariablesMap)
                       : serVarMap(ServerVariablesMap) {
    execute(cgiToExecute, env);
}

CgiManager::CgiManager(const CgiManager &copy) {
    *this = copy;
}

CgiManager::~CgiManager() {

}

CgiManager &CgiManager::operator=(const CgiManager &toAssign) {
    this->cgiResponse = toAssign.cgiResponse;
    return *this;
}

void CgiManager::execute(const std::string& cgiName, char **env) {
    int pid = fork();
    int fds[2];
	const std::string cgiFileName("cgiFile.txt");
	std::ofstream cgiFile(cgiFileName.c_str());
	std::streambuf *oldCout = std::cout.rdbuf();
    pipe(fds);
    if (pid == 0)
    {
        char *args[] ={const_cast<char *>(cgiName.c_str()), NULL};
       	std::cout.rdbuf(cgiFile.rdbuf()); //dup2 like
        execve(CGI_PATH, args, env);
        std::cerr << "CGI EXCEPTION\n";
        exit(2);
    } //todo: recuperer les informations du CGI et les enregistrer dans cgiResponse;
	std::cout.rdbuf(oldCout);
	convertCgiFileToCgiResponse(cgiFileName);
	cgiFile.close();
}

void CgiManager::sendEnvVarsToCgi() {
    { //todo: remove quand j'aurais la vrai map post-parsing/recuperation de la methode par le serveur
        serVarMap["REQUEST_METHOD="] = "[PH]Rqst Meth"; //contient la méthode HTTP utilisée pour effectuer la requête (par exemple, GET, POST, etc.)
        serVarMap["QUERY_STRING"] = "[PH]QUERY STR";    // contient la chaîne de requête de l'URL de la requête HTTP
        serVarMap["CONTENT_TYPE"] = "[PH]text/html";    // contient le type de contenu de la requête HTTP (par exemple, application/json, multipart/form-data, etc.)
        serVarMap["CONTENT_LENGTH"] = "42";         // contient la longueur du contenu de la requête HTTP
    }

    std::map<std::string, std::string>::iterator itMap = serVarMap.begin();
    for (; itMap != serVarMap.end() ; ++itMap) {
        putenv(const_cast<char *>((itMap->first + "=" + itMap->second).c_str()));

	}
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

//
// Created by tgriffit on 4/3/23.
//

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

void CgiManager::execute(std::string cgiName, char **env) {
    int pid = fork();
    int fds[2];

    pipe(fds);
    if (pid == 0)
    {
        close(fds[0]);
        char *args[] ={const_cast<char *>(cgiName.c_str()), NULL};
        dup2(fds[1], STDOUT_FILENO);
        execve(CGI_PATH, args, env);
        std::cerr << "CGI EXCEPTION\n";
        exit(2);
    } //todo: recuperer les informations du CGI et les enregistrer dans cgiResponse;
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

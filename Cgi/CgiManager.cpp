//
// Created by tgriffit on 4/3/23.
//

#include <fstream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
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

void CgiManager::execute(const std::string &cgiName) { //todo: modifier le env original
    int pid = fork();
    int fds_in[2], fds_out[2];
	const std::string cgiFileName("cgiFile.txt");

    convertEnvMap();
    char **env = convertEnvMap();


	std::ofstream cgiFile(cgiFileName.c_str());
	std::streambuf *oldCout = std::cout.rdbuf();
    //pipe(fds_in);
    pipe(fds_out);

    if (pid == 0)
    {
        close(fds_out[0]);
        dup2(fds_out[1], STDOUT_FILENO);
        close(fds_out[1]);
        std::string command = PHP_CMD " " + cgiName;
        std::cerr << "CGI is executed: " << std::boolalpha << std::system(command.c_str()) << std::endl;
        exit(EXIT_SUCCESS);

        //dup2(fds_in[0], STDIN_FILENO);
        /*int cgiFD = ::fileno(cgiFile);
        std::string fullExecutablePath(CGI_PATH + cgiName);
        char *args[] ={const_cast<char *>(fullExecutablePath.c_str()), NULL};
       	std::cout.rdbuf(cgiFile.rdbuf()); //dup2 like
        std::cerr << "CGI TO EXEC:" << args[0] << std::endl;

        execve("/bin/php", args, env);
        std::cerr << "<><><><<<<<>CGI EXCEPTION<>>>>>>>>>>>><><><><>\n";
        exit(2);*/
    } //todo: recuperer les informations du CGI et les enregistrer dans cgiResponse;
    else
    {
        close(fds_out[1]); // on ferme l'écriture du pipe dans le processus parent
        std::cout << "\nCHOCAPIC\n";
        char buffer[4096];
        ssize_t n;
        while ((n = read(fds_out[0], buffer, sizeof(buffer))) > 0) {
            // écriture de la sortie dans un fichier
            int outputFd = open("output.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
            write(outputFd, buffer, n);
            close(outputFd);
        }
        close(fds_out[0]);
    }
	std::cout.rdbuf(oldCout);
	convertCgiFileToCgiResponse(cgiFileName);

    for (size_t i = 0; env[i]; i++)
        delete[] env[i];
    delete[] env;
	cgiFile.close();
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

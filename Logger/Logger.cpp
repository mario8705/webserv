//
// Created by sansho on 21/03/23.
//

#include "Logger.hpp"

Logger::Logger(){
	logFile.open("logfile.txt", std::ios_base::app);
}

Logger::Logger(const Logger &copy)
{
	*this = copy;
}

Logger::~Logger() {
	if (logFile.is_open()) {
		logFile.close();
	}
}

Logger &Logger::operator=(const Logger &toAssign)
{
	(void)toAssign;
	return *this;
}

void Logger::log(const std::string &level){
	time_t now = std::time(NULL);
	char timestamp[20];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	std::string ip = "[PH]USER IP";
	std::string method = "[PH]METHOD";
	int errorCode = 420;
	std::string navInfos = "[PH]Navigator Infos";
	char c_level = static_cast<char>(std::tolower(level.at(0)));

	std::string severityColor;
	switch (c_level) {
		case 'd': severityColor = Blue; break;
		case 'i': severityColor = Green; break;
		case 'w': severityColor = Yellow; break;
		case 'e': severityColor = Red; break;
		default: severityColor = Reset;
	}


	switch (c_level) { //En remplaçant std::cout par logFile, les infos seront écrites dans un fichier (mais les couleurs seront perdues)
		case 'd': std::cout << severityColor << ip + " - - [" << timestamp << "] ";
		case 'i': std::cout << severityColor << "\"" + method + "\" ";
		case 'w': std::cout << severityColor << errorCode << " ";
		case 'e': std::cout << severityColor << navInfos << Reset << std::endl; break;
	}

}

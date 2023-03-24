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

void Logger::log(int level) {
    log(level, "");
}

void Logger::log(int level, const std::string &message) {
	time_t now = std::time(NULL);
	char timestamp[20];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	std::string ip = "[PH]USER IP";
	std::string method = "[PH]METHOD";
	int errorCode = 420;
	std::string navInfos = "[PH]Navigator Infos";

	std::string severityColor;
	switch (level) {
		case DEBUG: severityColor = Blue; break;
		case INFO: severityColor = Green; break;
		case WARNING: severityColor = Yellow; break;
		case ERROR: severityColor = Red; break;
		default: severityColor = Reset;
	}


	switch (level) { //En remplaçant std::cout par logFile, les infos seront écrites dans un fichier (mais les couleurs seront perdues)
		case DEBUG: std::cout << severityColor << ip + " - - [" << timestamp << "] ";
		case INFO: std::cout << severityColor << "\"" + method + "\" ";
		case WARNING: std::cout << severityColor << errorCode << " ";
		case ERROR: std::cout << severityColor << navInfos << Reset; break;
        default: std::cerr << "[LOGGER] Wrong log level given\n";
	}
    std::cout << ": " + message + "\n";
}

void Logger::setOutput(std::ostream &out) {

}

//
// Created by sansho on 21/03/23.
//

#ifndef WEBSERV_LOGGER_HPP
#define WEBSERV_LOGGER_HPP

#include <iostream>
#include <ctime>
#include <fstream>

#define Black "\u001b[30m"
#define Red "\u001b[31m"
#define Green "\u001b[32m"
#define Yellow "\u001b[33m"
#define Blue "\u001b[34m"
#define Magenta "\u001b[35m"
#define Cyan "\u001b[36m"
#define Reset "\u001b[37m"

class Logger
{
private:
	std::ofstream logFile;

public:
	Logger();
	Logger(const Logger &copy);
	~Logger();
	Logger &operator=(const Logger &toAssign);


	void log(const std::string& message);


};


#endif //WEBSERV_LOGGER_HPP

//
// Created by sansho on 21/03/23.
//

#include "Logger.hpp"

int main() {
	Logger logger;
	logger.log("debug");
	logger.log("info");
	logger.log("warning");
	logger.log("error");
	return 0;
}
//
// Created by sansho on 21/03/23.
//

#include "Logger.hpp"

int main() {
	Logger logger;
    logger.log(DEBUG, "Oui Oui baguette");
    logger.log(INFO, "");
    logger.log(WARNING, "");
    logger.log(ERROR, "Ceci est une erreur");
	return 0;
}
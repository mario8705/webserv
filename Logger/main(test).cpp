//
// Created by sansho on 21/03/23.
//

#include "Logger.hpp"
#include <stdio.h>

int main() {
	Logger logger;
	logger.log("debug");
	logger.log("info");
	logger.log("warning");
	logger.log("error");
  //  std::cout << "\e]8;;https://profile.intra.42.fr/users/twang\a\e[34mtwang\e[34m\e]8;;\a :star:\n\n" << std::endl;
    printf("\\e]8;;https://profile.intra.42.fr/users/twang\a\\etwang\\e[34m\\e]8;;\a :star:\n\n");

    return 0;
}
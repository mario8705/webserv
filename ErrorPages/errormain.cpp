//
// Created by sansho on 28/03/23.
//

#include <fstream>
#include "ErrorPageGenerator.hpp"

/**
 * Compile avec:
 c++ -Wno-deprecated *.cpp -o errorGen
 */
int main(void)
{
	ErrorPageGenerator pageErrorGenerator;
	std::ofstream example("errorpage.html");

	example << pageErrorGenerator.generateErrorPage(404) << std::endl;
	example.close();
}
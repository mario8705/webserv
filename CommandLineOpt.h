//
// Created by alavaud on 3/13/23.
//

#ifndef WEBSERV_COMMANDLINEOPT_H
#define WEBSERV_COMMANDLINEOPT_H
#include <string>
#include <vector>

class CommandLineOpt {
public:
    CommandLineOpt();

    bool Parse(int argc, char *argv[]);

private:
};


#endif //WEBSERV_COMMANDLINEOPT_H

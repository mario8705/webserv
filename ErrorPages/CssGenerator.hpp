//
// Created by tgriffit on 3/29/23.
//

#ifndef CSGENERATOR_HPP
#define CSSGENERATOR_HPP

#include <iostream>
#include <map>
#include <vector>

class CssGenerator {

public:
    CssGenerator();
    CssGenerator(const CssGenerator &copy);
    ~CssGenerator();
    CssGenerator &operator=(const CssGenerator &toAssign);

    std::map<std::string, std::string> generateCssBlocs(const std::string *blocs);
    std::string generateBloc(const std::string &blocName, std::map<std::string, std::vector<std::string>> &content);

};


#endif

//
// Created by tgriffit on 3/29/23.
//

#include "CssGenerator.hpp"

CssGenerator::CssGenerator() {}

CssGenerator::CssGenerator(const CssGenerator &copy) {

}

CssGenerator::~CssGenerator() {

}

CssGenerator &CssGenerator::operator=(const CssGenerator &toAssign) {
    return *this;
}

std::map<std::string, std::string> CssGenerator::generateCssBlocs(const std::string *blocs) {

    return nullptr;
}

std::string CssGenerator::generateBloc(const std::string &blocName, std::map<std::string, std::vector<std::string>> &content) {
    std::string result(blocName + " {\n");

    std::map<std::string, std::vector<std::string>>::iterator itCont = content.begin();

    for (; itCont != content.end(); ++itCont) {
        result += ' ' + itCont->first;

        std::vector<std::string>::iterator itValue = itCont->second.begin();
        for (; itValue != itCont->second.end() ; ++itValue) {
            result += *itValue + ' ';
        }

        result += ";\n";
    }

    result += "}\n";

    return result;
}

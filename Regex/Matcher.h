//
// Created by Alexis Lavaud on 06/05/2023.
//

#ifndef WEBSERV_MATCHER_H
#define WEBSERV_MATCHER_H
#include <string>
#include "RegexElement.h"

class Pattern;

class MatchResult {};

class Matcher
{
    friend class Pattern;

public:
    Matcher(Pattern *pattern, const std::string &input);
    ~Matcher();

    bool Match(MatchResult &result);

private:

    bool MatchElements(std::vector<RegexElement> &elements, size_t &pos, std::string &result);

    std::vector<RegexElement> &m_elements;
    std::string m_input;
    size_t m_head;
    size_t m_end;
};


#endif //WEBSERV_MATCHER_H

//
// Created by Alexis Lavaud on 05/05/2023.
//

#ifndef WEBSERV_PATTERN_H
#define WEBSERV_PATTERN_H
#include <string>
#include <vector>
#include "RegexElement.h"

class Matcher;

class Pattern
{
public:
    ~Pattern();

    bool Match(const std::string &input);
    Matcher *CreateMatcher(const std::string &input);

    static Pattern *Compile(const std::string &regex);

private:
    explicit Pattern(std::vector<RegexElement> &elements);

    static void DecodeElements(std::vector<RegexElement> &elements, const std::string &regex,
                               size_t head, size_t tail);
    static void DecodeRangeList(RegexElement &element,
                                const std::string &regex, size_t head, size_t tail);
    static size_t DecodeOccurenceModifier(RegexElement &element,
                                        const std::string &regex, size_t head);

    std::vector<RegexElement> m_elements;
};


#endif //WEBSERV_PATTERN_H

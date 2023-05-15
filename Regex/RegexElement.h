//
// Created by Alexis Lavaud on 05/05/2023.
//

#ifndef WEBSERV_REGEXELEMENT_H
#define WEBSERV_REGEXELEMENT_H
#include <vector>
#include <string>

struct Range
{
    int start;
    int end;

    Range(int start, int end)
            : start(start), end(end)
    {
    }
};

enum RegexElementType
{
    kRegexElementType_AssertStart,
    kRegexElementType_AssertEnd,
    kRegexElementType_Range,
    kRegexElementType_CaptureGroup,
};

struct RegexElement
{
    RegexElementType type;
    std::vector<Range> ranges;
    std::vector<RegexElement> elements;
    int min;
    int max;

    explicit RegexElement(RegexElementType type)
            : type(type)
    {
        min = 1;
        max = 1;
    }

    void AddRange(int start, int end)
    {
        ranges.push_back(Range(start, end));
    }

    void SetElements(std::vector<RegexElement> &elements)
    {
        elements.swap(this->elements);
    }

    void SetMinMax(int min, int max)
    {
        this->min = min;
        this->max = max;
    }

    bool MatchRange(int ch) const
    {
        size_t i;

        for (i = 0; i < ranges.size(); ++i)
        {
            if (ch >= ranges[i].start && ch <= ranges[i].end)
                return true;
        }
        return false;
    }
};

#endif //WEBSERV_REGEXELEMENT_H

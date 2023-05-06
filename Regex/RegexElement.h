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
};

struct RegexElement
{
    RegexElementType type;
    std::vector<Range> ranges;
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

    void SetMinMax(int min, int max)
    {
        this->min = min;
        this->max = max;
    }

    /*
    bool Match(const std::string &input, size_t &pos) const
    {
        switch (type)
        {
        case kRegexElementType_AssertStart:
            return (pos == 0);

        case kRegexElementType_AssertEnd:
            printf("%zu %zu\n", (pos + 1), input.size());
            return ((pos + 1) >= input.size());

        case kRegexElementType_Range:
            return MatchRange(input[pos++]);

        default:
            break;
        }
        return false;
    }*/

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

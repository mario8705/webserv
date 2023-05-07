//
// Created by Alexis Lavaud on 05/05/2023.
//

#include "Pattern.h"
#include <vector>
#include "RegexException.h"
#include "Matcher.h"

Pattern::Pattern(std::vector<RegexElement> &elements)
{
    m_elements.swap(elements);

    /*
    for (auto &&e : m_elements)
    {
        if (e.ranges.size() > 0)
        printf("[%c-%c]{%d,%d}\n", e.ranges[0].start, e.ranges[0].end, e.min, e.max);
        else
            printf("No range\n");
    }*/
}

Pattern::~Pattern()
{
}

bool Pattern::Match(const std::string &input)
{
    size_t i, j, cur;
    int occ;

    for (i = 0; i < input.size(); ++i)
    {
        cur = i;
        for (j = 0; j < m_elements.size(); ++j)
        {
            for (occ = 0; occ < m_elements[j].max; ++occ)
            {
                if (kRegexElementType_AssertStart == m_elements[j].type)
                {
                    if (cur != 0)
                        break ;
                }
                else if (kRegexElementType_AssertEnd == m_elements[j].type)
                {
                    if (cur != input.size())
                        break ;
                }
                else if (kRegexElementType_Range == m_elements[j].type && cur < input.size()) {
                    if (!m_elements[j].MatchRange(input[cur]))
                        break ;
                    ++cur;
                }
                else
                {
                    break ;
                }
            }
            if (occ < m_elements[j].min) {
                break;
            }
        }
        if (j == m_elements.size())
            return true;
    }
    return false;
}

Matcher *Pattern::CreateMatcher(const std::string &input)
{
    return new Matcher(m_elements, input);
}

Pattern *Pattern::Compile(const std::string &regex)
{
    size_t i;
    size_t pos;
    std::vector<RegexElement> elements;

    DecodeElements(elements, regex, 0, regex.size());
    return new Pattern(elements);
}

void Pattern::DecodeElements(std::vector<RegexElement> &elements, const std::string &regex,
                             size_t head, size_t tail)
{
    size_t pos;

    while (head < tail) {
        if (regex[head] == '^') {
            elements.push_back(RegexElement(kRegexElementType_AssertStart));
            ++head;
        } else if (regex[head] == '$') {
            elements.push_back(RegexElement(kRegexElementType_AssertEnd));
            ++head;
        } else if (regex[head] == '[') {
            do {
                pos = regex.find(']', head + 1);
            } while (std::string::npos != pos && regex[pos - 1] == '\\');
            if (std::string::npos == pos)
                throw RegexException("[ Character class missing closing bracket");
            RegexElement element(kRegexElementType_Range);
            DecodeRangeList(element, regex, head + 1, pos);
            elements.push_back(element);
            head = pos + 1;
        } else if (regex[head] == '(') {
            do {
                pos = regex.find(')', head + 1);
            } while (std::string::npos != pos && regex[pos - 1] == '\\');
            if (std::string::npos == pos)
                throw RegexException("");
            RegexElement element(kRegexElementType_CaptureGroup);
            std::vector<RegexElement> groupElements;
            DecodeElements(groupElements, regex, head + 1, pos);
            element.SetElements(groupElements);
            elements.push_back(element);
            head = pos + 1;
        } else {
            RegexElement element(kRegexElementType_Range);

            if (regex[head] == '\\') {
                if (++head >= tail)
                    throw RegexException("Pattern may not end with a trailing backslash");

                switch (regex[head]) {
                    /**
                     * Matches a digit (equivalent to [0-9])
                     */
                    case 'd':
                        element.AddRange('0', '9');
                        break;

                        /**
                         * Matches any whitespace character (equivalent to [\r\n\t\f\v ])
                         */
                    case 's':
                        element.AddRange(' ', ' ');
                        element.AddRange('\t', '\t');
                        element.AddRange('\r', '\r');
                        element.AddRange('\v', '\v');
                        element.AddRange('\n', '\n');
                        element.AddRange('\f', '\f');
                        break;

                    case '[':
                    case ']':
                    case '.':
                    case '(':
                    case ')':
                    case '^':
                    case '$':
                    case '{':
                    case '}':
                    case '+':
                    case '*':
                    case '?':
                        element.AddRange(regex[head], regex[head]);
                        break;

                    default:
                        throw RegexException("This token has no special meaning and has thus been rendered erroneous");
                }
            } else {
                element.AddRange(regex[head], regex[head]);
            }
            elements.push_back(element);
            ++head;
        }
        if (head < tail)
            head = DecodeOccurenceModifier(elements.back(), regex, head);
    }
}

void Pattern::DecodeRangeList(RegexElement &element,
                              const std::string &regex, size_t head, size_t tail)
{
    unsigned char start, end;

    while (head < tail)
    {
        /**
         * Handles -c (Less than)
         */
        if (regex[head] == '-' && (head + 1) < tail)
        {
            end = regex[head + 1];
            start = 0;
            head += 2;
        }
        /**
         * Handles c- and c-c (Greater than and range)
         */
        else if ((head + 1) < tail && regex[head + 1] == '-')
        {
            start = regex[head++];
            if ((head + 1) != tail)
                end = regex[++head];
            else
                end = 255;
            ++head;
        }
        /**
         * Handle c (Single character)
         */
        else
        {
            start = end = regex[head++];
        }
        element.AddRange(start, end);
    }
}

size_t Pattern::DecodeOccurenceModifier(RegexElement &element,
                                        const std::string &regex, size_t head)
{
    int min, max;
    size_t pos;
    size_t i;

    min = 1;
    max = 1;
    if (regex[head] == '+')
    {
        min = 1;
        max = 2147483647;
    }
    else if (regex[head] == '?')
    {
        min = 0;
        max = 1;
    }
    else if (regex[head] == '*')
    {
        min = 0;
        max = 2147483647;
    }
    else if (regex[head] == '{')
    {
        pos = regex.find('}', head + 1);
        if (std::string::npos != pos)
        {
            for (i = head + 1; i < pos; ++i)
            {
                printf("Occurence %c\n", regex[i]); /* TODO */
                //if (!std::isdigit(regex[i]))
                  //  break ;
            }
        }
    }
    else
    {
        return head;
    }
    if (kRegexElementType_Range != element.type && kRegexElementType_CaptureGroup != element.type)
    {
        throw RegexException("The preceding token is not quantifiable");
    }
    element.SetMinMax(min, max);
    return head + 1;
}


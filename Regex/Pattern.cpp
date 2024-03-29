//
// Created by Alexis Lavaud on 05/05/2023.
//

#include "Pattern.h"
#include <vector>
#include "RegexException.h"
#include "Matcher.h"

Pattern::Pattern()
{
}

Pattern::~Pattern()
{
}

bool Pattern::Match(const std::string &input)
{
    MatchResult result;

    Matcher m(this, input);
    return m.Match(result);
}

void Pattern::Compile(const std::string &regex)
{
    m_elements.clear();
    m_elements.reserve(16);
    DecodeElements(m_elements, regex, 0, regex.size());
}

bool Pattern::Matches(const std::string &pattern, const std::string &input)
{
    Pattern p;
    MatchResult res;
    bool r;

    p.Compile(pattern);
    Matcher m(&p, input);
    r = m.Match(res);
    return r;
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
            if ((head + 1) < tail)
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
                /* For the next version... */
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


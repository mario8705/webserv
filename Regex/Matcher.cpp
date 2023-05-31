//
// Created by Alexis Lavaud on 06/05/2023.
//

#include "Matcher.h"
#include "Pattern.h"

/**
 * TODO Groups have been nested too deeply, consider simplifying your pattern
 */

Matcher::Matcher(Pattern *pattern, const std::string &input)
    : m_elements(pattern->m_elements)
{
    m_input = input.c_str();
    m_head = 0;
    m_end = input.size();
}


Matcher::~Matcher()
{
}

bool Matcher::Match(MatchResult &)
{
    size_t pos;
    std::string res;

    for (; m_head <= m_end; )
    {
        pos = m_head;
        if (MatchElements(m_elements, pos, res))
        {
            m_head = pos;
            return true;
        }
        else
        {
            m_head++;
        }
    }
    return false;
}

bool Matcher::MatchElements(std::vector<RegexElement> &elements, size_t &pos, std::string &result)
{
    size_t start, i;
    int occ;

    start = pos;
    for (i = 0; i < elements.size(); ++i)
    {
        for (occ = 0; occ < elements[i].max; ++occ)
        {
            if (kRegexElementType_AssertStart == elements[i].type)
            {
                if (0 != pos)
                    break ;
            }
            else if (kRegexElementType_AssertEnd == elements[i].type)
            {
                if (pos != m_end)
                    break ;
            }
            else if (kRegexElementType_Range == elements[i].type)
            {
                if (pos >= m_end)
                    break ;
                if (!elements[i].MatchRange((unsigned char)m_input[pos]))
                    break ;
                ++pos;
            }
            else if (kRegexElementType_CaptureGroup == elements[i].type)
            {
                std::string groupResult;
                if (!MatchElements(elements[i].elements, pos, groupResult))
                {
                    break ;
                }
            }
        }
        if (occ < elements[i].min)
            break ;
    }
    if (elements.size() == i)
    {
        result.assign(m_input.c_str() + start, pos - start);
        return true;
    }
    return false;
}

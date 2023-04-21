//
// Created by Alexis Lavaud on 06/04/2023.
//

#ifndef WEBSERV_STRING_UTILS_HPP
#define WEBSERV_STRING_UTILS_HPP
#include <string>
#include <sstream>

namespace utils
{
    template <typename T>
    std::string to_string(const T &val)
    {
        std::stringstream ss;

        ss << val;
        return ss.str();
    }

    inline std::string &ltrim(std::string &s)
    {
        std::string::iterator it;

        for (it = s.begin(); it != s.end(); ++it)
        {
            if (!std::isspace(*it))
            {
                break ;
            }
        }
        s.erase(s.begin(), it);
        return s;
    }

    inline std::string &rtrim(std::string &s)
    {
        std::string::reverse_iterator it;

        for (it = s.rbegin(); it != s.rend(); ++it)
        {
            if (!std::isspace(*it))
            {
                break ;
            }
        }
        s.erase(it.base(), s.end());
        return s;
    }

    inline std::string &trim(std::string &s)
    {
        ltrim(s);
        rtrim(s);
        return s;
    }

    template <typename charT>
    inline std::basic_string<charT> &to_lower(std::basic_string<charT> &s)
    {
        typename std::basic_string<charT>::iterator it;

        for (it = s.begin(); it != s.end(); ++it)
        {
            *it = std::tolower(*it);
        }
        return s;
    }
}

#endif //WEBSERV_STRING_UTILS_HPP

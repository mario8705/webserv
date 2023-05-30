//
// Created by Alexis Lavaud on 06/04/2023.
//

#ifndef WEBSERV_STRING_UTILS_HPP
#define WEBSERV_STRING_UTILS_HPP
#include <string>
#include <sstream>


namespace ft
{
    template< class InputIt1, class InputIt2, class Compare >
    bool lexicographical_compare( InputIt1 first1, InputIt1 last1,
                                  InputIt2 first2, InputIt2 last2, Compare comp )
    {
        for (; (first1 != last1) && (first2 != last2); ++first1, (void) ++first2)
        {
            if (comp(*first1, *first2))
                return true;
            if (comp(*first2, *first1))
                return false;
        }
        return (first1 == last1) && (first2 != last2);
    }
}

struct case_insensitive_less
{
    struct case_insensitive_compare
    {
        bool operator()(const unsigned char &c1, const unsigned char &c2) const
        {
            return std::tolower(c1) < std::tolower(c2);
        }
    };

    bool operator()(const std::string &lhs, const std::string &rhs) const
    {
        return ft::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                case_insensitive_compare()
        );
    }
};


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

    inline bool ends_with(const std::string &s, const std::string &suffix)
    {
        if (s.size() >= suffix.size())
            return !s.compare(s.size() - suffix.size(), suffix.length(), suffix);
        return false;
    }
}

#endif //WEBSERV_STRING_UTILS_HPP

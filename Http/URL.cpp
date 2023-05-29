//
// Created by Alexis Lavaud on 04/04/2023.
//

#include "URL.h"
#include <iostream>

URL::URL()
{
}

URL::URL(std::string path)
    : m_path(path)
{
}

URL::~URL()
{
}

std::string URL::GetAbsolutePath(const std::string &root) const
{
    std::string absolutePath(root);
    size_t cur, sep;
    std::string component;

    cur = 0;
    do {
        sep = m_path.find('/', cur);

        if (std::string::npos != sep)
            component = m_path.substr(cur, sep - cur);
        else
            component = m_path.substr(cur);

        if (".." == component)
            return ""; /* TODO throw exception ?, return a boolean ? */

        if (absolutePath.empty() || absolutePath[absolutePath.size() - 1] != '/')
            absolutePath.insert(absolutePath.end(), '/');
        absolutePath.insert(absolutePath.end(), component.begin(), component.end());

        cur = sep + 1;
    } while (std::string::npos != sep);

    return absolutePath;
}

URL URL::ParseURL(const std::string &url)
{
    size_t querySep;
    std::string path;

    if (std::string::npos != (querySep = url.find_first_of('?')))
    {
        path = url.substr(0, querySep);
    }
    else
    {
        path = url;
    }
    return URL(path);
}

static int hexval(int ch)
{
    if (ch >= '0' && ch <= '9')
        return (ch - '0');
    else if (ch >= 'a' && ch <= 'f')
        return (ch - 'a');
    else if (ch >= 'A' && ch <= 'F')
        return (ch - 'A');
    return -1;
}

std::string URL::Decode(const std::string &s) {
    std::string res;
    size_t i;
    int ch;
    int val[2];

    for (i = 0; i < s.size(); )
    {
        ch = (unsigned char)s[i++];
        if (ch == '%' &&
            (i + 2) < s.size() &&
            (val[0] = hexval(s[i + 0])) >= 0 &&
            (val[1] = hexval(s[i + 1])) >= 0)
        {
            ch = (val[0] << 4) | val[1];
            i += 2;
        }
        res.insert(res.end(), static_cast<char>(ch));
    }
 //   printf("Decoded '%s' into '%s'\n", s.c_str(), res.c_str());
    return res;
}

std::string URL::Encode(const std::string &s)
{
    std::string res;
    size_t i;
    int ch;

    for (i = 0; i < s.size(); ++i)
    {
        ch = (unsigned char)s[i];

        res.insert(res.end(), static_cast<char>(ch));
    }
    return res;
}

URL::URL(const std::string &protocol, const std::string &host, const std::string &path, const std::string &query)
    : m_protocol(protocol), m_host(host), m_path(path), m_query(query)
{
}

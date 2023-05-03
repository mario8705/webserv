//
// Created by Alexis Lavaud on 04/04/2023.
//

#ifndef WEBSERV_URL_H
#define WEBSERV_URL_H
#include <string>
#include <map>

class URL
{
public:
    URL();
    ~URL();

    static std::string Decode(const std::string &s);
    static std::string Encode(const std::string &s);

    static URL ParseURL(const std::string &url);

    std::string m_path;
    std::map<std::string, std::string> m_queryParams;
    std::string m_rawQueryParams;
};


#endif //WEBSERV_URL_H

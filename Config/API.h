//
// Created by Robin Geral on 17/03/2023.
//

#ifndef WEBSERV_API_H
#define WEBSERV_API_H
#include <string>
#include <vector>
#include <map>

class ConfigBlock
{
public:
    std::string m_name;
    std::vector<std::string> m_params;
    std::map<std::string, std::string> m_rules;
    std::vector<ConfigBlock *> m_subBlocks;
};

class Config
{
public:
    std::vector<ConfigBlock *> m_blocks;
};

#endif //WEBSERV_API_H

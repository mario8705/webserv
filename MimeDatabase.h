//
// Created by Alexis Lavaud on 07/05/2023.
//

#ifndef WEBSERV_MIMEDATABASE_H
#define WEBSERV_MIMEDATABASE_H
#include <map>
#include <string>
#include "string_utils.hpp"

class MimeDatabase
{
public:
    typedef std::map<std::string, std::string, case_insensitive_less> tMimeTypesDatabase;

    explicit MimeDatabase(const std::string &defaultType);
    ~MimeDatabase();

    bool RegisterType(const std::string &ext, const std::string &mime);
    void SetDefaultType(const std::string &defaultType);

    std::string GetMimeType(const std::string &filename) const;

private:
    tMimeTypesDatabase m_database;
    std::string m_defaultType;
};


#endif //WEBSERV_MIMEDATABASE_H

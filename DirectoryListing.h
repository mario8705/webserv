//
// Created by Alexis Lavaud on 30/05/2023.
//

#ifndef WEBSERV_DIRECTORYLISTING_H
#define WEBSERV_DIRECTORYLISTING_H
#include <string>
#include <ostream>
#include <dirent.h>

class DirectoryListing
{
public:
    DirectoryListing(DIR *dirp, const std::string &baseUrl);

    void Write(std::ostream &out);

private:
    DIR *m_dirp;
    std::string m_baseUrl;
};


#endif //WEBSERV_DIRECTORYLISTING_H

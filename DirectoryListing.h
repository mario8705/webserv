//
// Created by Alexis Lavaud on 30/05/2023.
//

#ifndef WEBSERV_DIRECTORYLISTING_H
#define WEBSERV_DIRECTORYLISTING_H
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <dirent.h>

class DirectoryListing
{
public:
    DirectoryListing(DIR *dirp, const std::string &urlPath, const std::string &realPath);

    void Write(std::ostream &out);

private:
    DIR *m_dirp;
    std::string m_urlPath;
    std::string m_realPath;
};


#endif //WEBSERV_DIRECTORYLISTING_H

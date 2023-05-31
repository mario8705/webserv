//
// Created by Alexis Lavaud on 30/05/2023.
//

#include "DirectoryListing.h"

DirectoryListing::DirectoryListing(DIR *dirp, const std::string &baseUrl)
    : m_dirp(dirp), m_baseUrl(baseUrl)
{
}

void DirectoryListing::Write(std::ostream &out)
{
    struct dirent *dent;
    std::string path;

    path = m_baseUrl;
    if (!path.empty() && path[path.size() - 1] != '/')
        path.append("/");

    out << "<!DOCTYPE html>\n"
           "<html lang=\"en\">\n"
           "<head>\n"
           "    <meta charset=\"UTF-8\">\n"
           "    <title>Directory listing of " << m_baseUrl << "</title>\n"
           "    <style>\n"
           "        body {\n"
           "            font-family: system-ui, -apple-system, \"Verdana\", sans-serif;\n"
           "            margin: 0 10em;\n"
           "        }\n"
           "\n"
           "        h1 {\n"
           "            text-align: center;\n"
           "            color: #333;\n"
           "            margin: 2em 0;\n"
           "            font-weight: 300;\n"
           "        }\n"
           "\n"
           "        th {\n"
           "            text-align: left;\n"
           "        }\n"
           "\n"
           "        th, td {\n"
           "            padding: 1em;\n"
           "        }\n"
           "\n"
           "        table {\n"
           "            width: 40em;\n"
           "            margin: 0 auto;\n"
           "            border-spacing: 0;\n"
           "            font-weight: 300;\n"
           "        }\n"
           "\n"
           "        th {\n"
           "            font-weight: 400;\n"
           "        }\n"
           "\n"
           "        td + td, th + th {\n"
           "            text-align: right;\n"
           "        }\n"
           "\n"
           "        tbody tr {\n"
           "            transition: background 200ms ease-in;\n"
           "        }\n"
           "\n"
           "        tbody tr:hover {\n"
           "            background-color: #F5F5F5;\n"
           "        }\n"
           "\n"
           "        a {\n"
           "            color: inherit;\n"
           "            transition: color 200ms ease-in;\n"
           "            text-decoration: underline;\n"
           "        }\n"
           "\n"
           "        a:hover {\n"
           "            color: #555;\n"
           "        }\n"
           "    </style>\n"
           "</head>\n"
           "<body>\n"
           "    <h1>Directory listing of " << m_baseUrl << "</h1>\n"
           "    <table>\n"
           "        <thead>\n"
           "        <tr>\n"
           "            <th>File Name</th>\n"
           "            <th>File Size</th>\n"
           "        </tr>\n"
           "        </thead>\n"
           "        <tbody>\n";

    while ((dent = readdir(m_dirp)) != NULL) {
        if (!std::strcmp(dent->d_name, ".")) {
            continue;
        }

        std::string href = path + dent->d_name;

        if (!std::strcmp(dent->d_name, ".."))
        {
            if (path != "/") {
                printf("Path: %s\n", path.c_str());
                href = path.substr(0, path.size() - 1);
                size_t l = href.find_last_of('/');
                if (std::string::npos == l) {
                    continue;
                }
                href = href.substr(0, l + 1);
            }
            else {
                continue;
            }
        }
        out << "            <tr>\n"
               "                <td>\n"
               "                    <a href=\"" << href << "\">"
               << dent->d_name << "</a></td>\n"
               "                <td>3M</td>\n"
               "            </tr>\n";
    }

    out << "        </tbody>\n"
           "    </table>\n"
           "</body>\n"
           "</html>";
}

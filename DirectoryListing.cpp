//
// Created by Alexis Lavaud on 30/05/2023.
//

#include "DirectoryListing.h"
#include <sys/stat.h>

DirectoryListing::DirectoryListing(DIR *dirp, const std::string &urlPath, const std::string &realPath)
    : m_dirp(dirp), m_urlPath(urlPath), m_realPath(realPath)
{
}

static void WriteFileSize(std::ostream &out, const std::string &path)
{
    struct stat st;
    off_t size;
    std::string suffix;

    if (stat(path.c_str(), &st) < 0)
    {
        out << "-";
        return ;
    }
    size = st.st_size;
    suffix = "b";
    if (size > 1024)
    {
        size /= 1024;
        suffix = "K";
    }
    if (size > 1024)
    {
        size /= 1024;
        suffix = "M";
    }
    if (size > 1024)
    {
        suffix = "G";
        size /= 1024;
    }
    out << size << suffix;
}

void DirectoryListing::Write(std::ostream &out)
{
    struct dirent *dent;
    std::string path;

    path = m_realPath;
    if (!path.empty() && path[path.size() - 1] != '/')
        path.append("/");

    out << "<!DOCTYPE html>\n"
           "<html lang=\"en\">\n"
           "<head>\n"
           "    <meta charset=\"UTF-8\">\n"
           "    <title>Directory listing of " << m_urlPath << "</title>\n"
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
           "    <h1>Directory listing of " << m_urlPath << "</h1>\n"
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

        std::string href = m_urlPath;
        if (!href.empty() && href[href.size() - 1] != '/')
            href.append("/");
        href.append(dent->d_name);

        if (!std::strcmp(dent->d_name, ".."))
        {
            size_t l;

            if (m_urlPath == "/")
                continue;

            href = m_urlPath.substr(0, m_urlPath.size() - 1);

            if (std::string::npos == (l = href.find_last_of('/')))
                continue;
            href = href.substr(0, l + 1);
        }
        out << "            <tr>\n"
               "                <td>\n"
               "                    <a href=\"" << href << "\">"
               << dent->d_name << "</a></td>\n"
                                  "                <td>";
        WriteFileSize(out, "/etc/resolve.conf");
        out << "</td>\n"
               "            </tr>\n";
    }

    out << "        </tbody>\n"
           "    </table>\n"
           "</body>\n"
           "</html>";
}

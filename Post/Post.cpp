//
// Created by Robin Geral on 02/05/2023.
//

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <map>

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string str = s;
    std::string token;
    size_t pos = 0;
    while ((pos = str.find(delimiter)) != std::string::npos)
    {
        token = str.substr(0, pos);
        tokens.push_back(token);
        str.erase(0, pos + 1);
    }
    tokens.push_back(str);
    return tokens;
}

int checklenght(std::vector<std::string> lines)
{
    size_t pos;
    std::string lenghtline;
    std::string tmp;
    int result;


    for (size_t i = 0; i < lines.size(); i++)
    {
        pos = lines[i].find("Content-Length:");
        if (pos != std::string::npos)
        {
            lenghtline = lines[i];
            break;
        }
    }
    tmp = lenghtline.substr(16);
    result = atoi(tmp.c_str());
    return(result);
}

void bodyparsing (std::string body)
{
    std::vector<std::string> v_body;
    std::map<std::string, std::string> m_body;
    std::string html_table;
    html_table = "<table border = 1>";
    v_body = split(body, '&');
    for (size_t i = 0; i < v_body.size(); i++)
    {
        std::vector<std::string> splitted_body = split(v_body[i], '=');
        for (size_t j = 0; j < splitted_body.size(); j++)
        {
            m_body[splitted_body[0]] = splitted_body[1];
        }
        splitted_body.clear();
    }
    for (std::map<std::string, std::string>::iterator it = m_body.begin(); it != m_body.end(); ++it)
    {
        std::cout << it->first << " = " << it->second << std::endl;
    }

}

void handle_connection(int fd)
{
    char buf[256];
    ssize_t n;
    std::string str;
    std::string line;
    std::vector<std::string > lines;
    size_t pos;
    std::string body;
    bool emptyLine = false;

    while (!emptyLine && (n = read(fd, buf, sizeof(buf))) > 0)
    {
        str.insert(str.end(), buf, buf + n);

        while ((pos = str.find_first_of('\n')) != std::string::npos)
        {
            line.clear();
            line.insert(line.begin(), str.begin(), str.begin() + pos);
            str.erase(str.begin(), str.begin() + pos + 1);

            if (line.size() > 0 && line[line.length() - 1] == '\r')
                line.erase(line.begin() + line.length() - 1, line.end());

            lines.push_back(line);
            if (line.empty())
            {
                emptyLine = true;
                break ;
            }
        }
    }
    int contentLenght = checklenght(lines);
    while (str.size() < contentLenght && (n = read(fd, buf, sizeof(buf))) > 0)
    {
        str.insert(str.end(), buf, buf + n);
    }
    body = str;
    bodyparsing(body);
}

void loop(int sockfd)
{
    int fd;
    struct sockaddr_in sin;
    socklen_t socklen;
    char buffer[124];
    size_t n;
    size_t pos;
    std::string line;
    std:: string tmp;

    while (1)
    {
        socklen = sizeof(sin);
        fd = accept(sockfd, (struct sockaddr *)&sin, &socklen);
        if (fd < 0)
        {
            perror("accept");
            return ;
        }
        printf("New connection %d\n", fd);
        handle_connection(fd);
        close(fd);
    }
}

int main()
{
    struct sockaddr_in sin;
    int sockfd;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(8080);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    int ok = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(ok));

    if (bind(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("bind");
        return 1;
    }

    if (listen(sockfd, 5) < 0)
    {
        perror("listen");
        return 1;
    }

    loop(sockfd);

    close(sockfd);
    return 0;
}
//
// Created by tgriffit on 3/9/23.
//

#ifndef HTMLGENERATOR_HPP
#define HTMLGENERATOR_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <error.h>
#include <cerrno>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <strstream>
#include <map>
#include <iomanip>
#include <sstream>

/*
#define SSTR( x ) dynamic_cast< std::ostringstream & >( (std::ostringstream() << std::dec << x) ).str()
#define MAX_CHAR_BUFFER 16384
*/

class HtmlGenerator {

    std::string htmlPage;
	std::string request;
    std::string header;

public:
    HtmlGenerator();
    HtmlGenerator(const std::string &html, const std::string &request);
    HtmlGenerator(const HtmlGenerator &copy);
    ~HtmlGenerator();
    HtmlGenerator &operator=(const HtmlGenerator &toAssign);

    std::string generateHeader(size_t nbMeta, const std::string& pageTitle);
    std::string generateBody(const std::string& content);
    std::string generatePage(std::string header, std::string body);
	std::string generateTableRow(const std::string &text);
	std::string generateTableDesc(const std::string &text);
    std::string generateTableRow(const std::string &text, const std::string &qualif);
    std::string generateTableDesc(const std::string &text, const std::string &qualif);
    std::string insertImg(const std::string &link);
    std::string insertImg(const std::string &link, const std::string &qualif);
    std::string generateStyleCss(const std::string *content);

};


#endif

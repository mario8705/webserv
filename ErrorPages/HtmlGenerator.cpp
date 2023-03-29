
#include <fstream>
#include "HtmlGenerator.hpp"
#include <string>

HtmlGenerator::HtmlGenerator(){

}

HtmlGenerator::HtmlGenerator(const std::string &html, const std::string &request) : htmlPage(html), request(request)
{

}

HtmlGenerator::HtmlGenerator(const HtmlGenerator &copy): header(copy.header){

}
HtmlGenerator::~HtmlGenerator() {

}

HtmlGenerator &HtmlGenerator::operator=(const HtmlGenerator &toAssign) {
    this->htmlPage = toAssign.htmlPage;
    this->request = toAssign.request;
    this->header = toAssign.header;
    return *this;
}

std::string HtmlGenerator::generateHeader(size_t nbMeta, const std::string& pageTitle)
{
    header = "<head>\n<meta ";

	if (nbMeta > 3)
		nbMeta = 3;
    switch (nbMeta) {
        case 3: header += "charset=\"utf-8\" ";
        case 2: header += "name=\"viewport\"";
        case 1: header += "content=\"width=device-width\"";
        default: "";
    }
    header += ">\n<title>" + pageTitle + "</title>\n</head>\n";
	return header;
}

static std::string generateP(const std::string &text)
{
    return "<p>" + text + "</p>\n";
}

static std::string generateP(const std::string &text, const std::string qualif)
{
    return "<p " + qualif + ">" + text + "</p>\n";
}

std::string HtmlGenerator::generateTableRow(const std::string &text)
{
    return "<tr>\n" + text + "</tr>\n";
}

std::string HtmlGenerator::generateTableRow(const std::string &text, const std::string &qualif)
{
    return "<tr " + qualif + ">\n" + text + "</tr>\n";
}

std::string HtmlGenerator::generateTableDesc(const std::string &text)
{
    return "<td>" + text + "</td>\n";
}

std::string HtmlGenerator::generateTableDesc(const std::string &text, const std::string &qualif)
{
    return "<td " + qualif + ">" + text + "</td>\n";
}

std::string HtmlGenerator::generateBody(const std::string& content)
{
	std::string body = "<body>\n";
	body += '\t' + content;
	body += "</body>\n";
	return body;
}

std::string HtmlGenerator::generatePage(std::string header, std::string body)
{
	std::string result("<!DOCTYPE html>\n<html>\n");

	result += '\t' + header;
	result += '\t' + body;

	result += "</html>\n";
	return result;
}

std::string HtmlGenerator::insertImg(const std::string &link) {
    return std::string("<img src=\"" + link + "\">\n");
}

std::string HtmlGenerator::insertImg(const std::string &link, const std::string &qualif) {
    return std::string("<img " + qualif + " src=\"" + link + "\">\n");
}

std::string HtmlGenerator::generateStyleCss(const std::string *content) {
    std::string result("<style>\n");

    for (int i = 0; i < content->size(); ++i) {
        result += ' ' + content[i] + '\n';
    }

    result += "</style>\n";
    return result;
}

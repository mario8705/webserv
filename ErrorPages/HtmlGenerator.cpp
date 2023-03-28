
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
    return "<p>" + text + "</p>";
}

std::string HtmlGenerator::generateTableRow(const std::string &text)
{
    return "<tr>\n" + text + "</tr>\n";
}

std::string HtmlGenerator::generateTableDesc(const std::string &text)
{
    return "<td>" + text + "</td>\n";
}


std::string HtmlGenerator::generateBody(const std::string& content)
{
	std::string body = "<body>\n";
	body += content;
	body += "</body>\n";
	return body;
}

std::string HtmlGenerator::generatePage(std::string header, std::string body)
{
	std::string result("<!DOCTYPE html>\n<html>\n");

	result += header;
	result += body;

	result += "</html>\n";
	return result;
}

//
// Created by sansho on 28/03/23.
//

#include "ErrorPageGenerator.hpp"

ErrorPageGenerator::ErrorPageGenerator(): errorCode(200)
{
	generatePage(generateHeader(3, "Error"), generateBody("Unknown Error"));
}

ErrorPageGenerator::ErrorPageGenerator(int errorCode): errorCode(errorCode)
{
	generateErrorPage();
}

ErrorPageGenerator::ErrorPageGenerator(const ErrorPageGenerator &copy)
 : HtmlGenerator(copy) {
	this->errorCode = copy.errorCode;
	*this = copy;
}

ErrorPageGenerator::~ErrorPageGenerator()
= default;

std::string ErrorPageGenerator::createErrorTitle() const
{
	std::string result("<h1> Error ");

	result += std::to_string(errorCode) + " - \n";

	switch (errorCode) {
		case 403: result += "Nope, you CAN'T go there !"; break;
		case 404: result += "Oh no, this page doesn't exists! ☹"; break;
		case 500: result += "Internal Server Error... Oopsie!"; break;
		case 503: result += "The server needs to relax..."; break;
		default: result += "...Hum...This is something we haven't think about...or have we ?";
	}
	result += "</h1>";

	return result;
}

std::string ErrorPageGenerator::createErrorDesc() const
{
	std::string result("<h2>");

	switch (errorCode) {
		case 403: result += "You should try to convince our server admins to give you some rights...Good luck!"; break;
		case 404: result += "You tried to access to somewhere which doesn't exist (like your future retreat)";
		result += "<p><img src=\"https://static1.cbrimages.com/wordpress/wp-content/uploads/2019/08/fullmetal-alchemist-nina-tucker.jpg\"></p>";break;
		case 500: result += "We promise, it's us this time, not you. You don't have to change."; break;
		case 503: result += "Well, it seems to be coffee time!"; break;
		default: result += "Nice try!";
	}
	result += "</h2>";

	return result;
}

std::string ErrorPageGenerator::generateErrorPage()
{
	std::string header = generateHeader(3, "Error " + std::to_string(errorCode));
	std::string title = createErrorTitle();
	std::string body = createErrorDesc();

	return (generatePage(header, generateBody(title + body)));
}

std::string ErrorPageGenerator::generateErrorPage(int error)
{
	int tmpErrCode = errorCode;

	this->errorCode = error;
	std::string result(generateErrorPage());
	this->errorCode = tmpErrCode;

	return result;
}

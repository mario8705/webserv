//
// Created by sansho on 28/03/23.
//

#ifndef ERRORPAGEGENERATOR_HPP
#define ERRORPAGEGENERATOR_HPP

#include "HtmlGenerator.hpp"
#include "CssGenerator.hpp"

class ErrorPageGenerator: public HtmlGenerator, public CssGenerator
{
private:
	int errorCode;
	std::string createErrorTitle() const;
	std::string createErrorDesc() const;

public:
	ErrorPageGenerator();
	ErrorPageGenerator(int errorCode);
	ErrorPageGenerator(const ErrorPageGenerator &copy);
	~ErrorPageGenerator();

	std::string generateErrorPage();
	std::string generateErrorPage(int error);
};


#endif

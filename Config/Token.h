#ifndef WEBSERV_TOKEN_H
#define WEBSERV_TOKEN_H
#include <string>
#include <vector>
#include "TokenType.h"

class Token {
public:
    Token(std::string token, TokenType tokentype);
    ~Token();

    TokenType getType();
    std::string getTypeString();
    std::string getToken();

    static void tokenization(std::vector<Token *>*tokens);

private:
    TokenType _tokenType;
    std::string _Token;
};


#endif //WEBSERV_TOKEN_H

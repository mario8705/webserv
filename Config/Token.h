#ifndef WEBSERV_TOKEN_H
#define WEBSERV_TOKEN_H
#include <string>
#include <vector>
#include <ostream>
#include "TokenType.h"

class Token {
public:
    Token(std::string token, TokenType tokentype);
    ~Token();

    TokenType getType() const;
    std::string getToken() const;

    static void tokenization(std::vector<Token *>*tokens);

private:
    TokenType _tokenType;
    std::string _Token;
};

inline std::ostream &operator<<(std::ostream &os, const Token &token)
{
    static const char* typeNames[] = {"ident", "left bracket", "right bracket", "semicolon", "string"};
    os << typeNames[token.getType()];
    return os;
}

#endif //WEBSERV_TOKEN_H

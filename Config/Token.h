#ifndef WEBSERV_TOKEN_H
#define WEBSERV_TOKEN_H
#include <string>
#include <vector>
#include <ostream>
#include "TokenType.h"

class Token {
public:
    Token(const std::string &token, TokenType tokenType, int line, int column);
    ~Token();

    TokenType getType() const;
    std::string getToken() const;

    static bool tokenization(const std::string &filename, std::vector<Token *> &tokens);

private:
    TokenType _tokenType;
    std::string _token;
    int m_line;
    int m_column;
};

inline std::ostream &operator<<(std::ostream &os, const Token &token)
{
    static const char* typeNames[] = {"ident", "left bracket", "right bracket", "semicolon", "string"};
    os << typeNames[token.getType()];
    return os;
}

#endif //WEBSERV_TOKEN_H

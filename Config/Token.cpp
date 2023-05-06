#include "Token.h"
#include <fstream>

Token::Token(std::string token, TokenType tokentype)
    : _Token(token), _tokenType(tokentype)
{
}

Token::~Token()
{
}

TokenType Token::getType() const
{
    return _tokenType;
}

std::string Token::getToken() const
{
    return _Token;
}

bool Token::tokenization(const std::string &filename, std::vector<Token *> &tokens)
{
    std::ifstream file(filename);
    std::string stringline;
    std::string line;
    char ch;

    if (!file.is_open())
        return false;
    while (file.get(ch))
    {
        if (std::isspace(ch))
        {
            if (!line.empty())
                tokens.push_back(new Token(line, kTokenType_Ident));
            line.clear();
            while (std::isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == ';')
        {
            if (!line.empty())
                tokens.push_back(new Token(line, kTokenType_Ident));
            line.clear();
            tokens.push_back(new Token(";", kTokenType_Semicolon));
            file.get(ch);
            while (std::isspace(ch) && !file.eof())
            {
                file.get(ch);
            }
        }
        if (ch == '#')
        {
            while (ch != '\n' && !file.eof())
                file.get(ch);
            while (std::isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == '{')
        {
            if (!line.empty())
                tokens.push_back(new Token(line, kTokenType_Ident));
            tokens.push_back(new Token("{", kTokenType_LeftBracket));
            line.clear();
            while (std::isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == '}')
        {
            tokens.push_back(new Token("}", kTokenType_RightBracket));
            line.clear();
            while (std::isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == '\'')
        {
            stringline += ch;
            file.get(ch);
            while (ch != '\'' && !file.eof())
            {
                stringline += ch;
                file.get(ch);
            }
            stringline += ch;
            tokens.push_back(new Token(stringline, kTokenType_String));
            stringline.clear();
            while (std::isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch != ';' && ch != '\n' && ch != '#' && ch!= '\'' && ch != '{' && ch != '}' && !file.eof())
            line += ch;
    }
    return true;
}

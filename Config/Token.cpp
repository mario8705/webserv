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

void Token::tokenization(std::vector<Token *> *tokens)
{
    char ch;
    std::string line;
    std::string stringline;
    std::string filename = "config.conf";
    std::ifstream file(filename.c_str());

    while (file.get(ch))
    {
        if (isspace(ch))
        {
            if (!line.empty())
                tokens->push_back(new Token(line, kTokenType_Ident));
            line.clear();
            while(isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == ';')
        {
            if (!line.empty())
                tokens->push_back(new Token(line, kTokenType_Ident));
            line.clear();
            tokens->push_back(new Token(";", kTokenType_Semicolon));
            file.get(ch);
            while(isspace(ch) && !file.eof())
            {
                file.get(ch);
            }
        }
        if (ch == '#')
        {
            while (ch != '\n' && !file.eof())
                file.get(ch);
            while(isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == '{')
        {
            if (!line.empty())
                tokens->push_back(new Token(line, kTokenType_Ident));
            tokens->push_back(new Token("{", kTokenType_LeftBracket));
            line.clear();
            while(isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == '}')
        {
            tokens->push_back(new Token("}", kTokenType_RightBracket));
            line.clear();
            while(isspace(ch) && !file.eof())
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
            tokens->push_back(new Token(stringline, kTokenType_String));
            stringline.clear();
            while(isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch != ';' && ch != '\n' && ch != '#' && ch!= '\'' && ch != '{' && ch != '}' && !file.eof())
            line += ch;
    }
}

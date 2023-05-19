#include "Token.h"
#include <fstream>

Token::Token(const std::string &token, TokenType tokentype, int line, int column)
    : _token(token), _tokenType(tokentype), m_line(line), m_column(column)
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
    return _token;
}

bool Token::tokenization(const std::string &filename, std::vector<Token *> &tokens)
{
    std::ifstream file(filename.c_str());
    std::string line;
    char ch;
    int nline;
    int col;

    if (!file.is_open())
        return false;
    nline = 0;
    col = 0;
    while (file.get(ch))
    {
        ++col;
        /* Consume all white spaces */
        if (std::isspace(ch))
        {
            if ('\n' == ch)
                ++nline, col = 0;
        }
        else if (ch == ';')
        {
            tokens.push_back(new Token(";", kTokenType_Semicolon, nline, 0));
        }
        else if (ch == '#')
        {
            while (file.get(ch))
            {
                if ('\n' == ch)
                    break ;
            }
            file.unget();
        }
        else if (ch == '{')
        {
            tokens.push_back(new Token("{", kTokenType_LeftBracket, nline, 0));
        }
        else if (ch == '}')
        {
            tokens.push_back(new Token("}", kTokenType_RightBracket, nline, 0));
        }
        else if (ch == '\'')
        {
            line.clear();
            while (file.get(ch))
            {
                ++col;
                if ('\'' == ch)
                    break ;
                if ('\n' == ch)
                {
                    /* TODO throw error */
                }
                line.insert(line.end(), ch);
            }
            if (file.eof())
            {
                /* TODO throw error */
            }
            tokens.push_back(new Token(line, kTokenType_String, nline, 0));
        }
        else
        {
            line.clear();
            /* TODO increment col here */
            do {
                if (';' == ch || std::isspace(ch) || '#' == ch || '{' == ch || '}' == ch)
                {
                    file.unget();
                    break ;
                }
                line.insert(line.end(), ch);
            } while (file.get(ch));
            tokens.push_back(new Token(line, kTokenType_Ident, nline, 0));
        }
    }
    return true;
}

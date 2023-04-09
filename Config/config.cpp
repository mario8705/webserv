#include "config.hpp"
Token::Token(std::string token, TokenType tokentype) : _Token(token), _tokenType(tokentype)
{
    //std::cout << "Token creation\n";
}
Token::~Token() {}
TokenType Token::getType() {return(_tokenType);}
std::string Token::getToken() {return (_Token);}

std::string Token::getTypeString() {
    static const char* typeNames[] = {"ident", "left bracket", "right bracket", "semicolon", "string"};
    return typeNames[_tokenType];
}
int main()
{
    char ch;
    std::vector<Token *> tokens;
    std::string line;
    std::string stringline;
    std::string filename = "../config.conf";
    std::ifstream file(filename.c_str());

    while (file.get(ch))
    {
        if (isspace(ch))
        {
            if (!line.empty())
                tokens.push_back(new Token(line, ident));
            //tokens.push_back(new Token(line, ident));
            line.clear();
            while(isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == ';')
        {
            if (!line.empty())
                tokens.push_back(new Token(line, ident));
            line.clear();
            tokens.push_back(new Token(";", semicolon));
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
            tokens.push_back(new Token("{", leftbracket));
            line.clear();
            while(isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == '}')
        {
            tokens.push_back(new Token("}", rightbracket));
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
            tokens.push_back(new Token(stringline, string));
            stringline.clear();
            while(isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch != ';' && ch != '\n' && ch != '#' && ch!= '\'' && ch != '{' && ch != '}' && !file.eof())
            line += ch;
    }
    if (!line.empty())
        tokens.push_back(new Token(line, ident));
    for (size_t i = 0; i < tokens.size(); i++)
    {
        std::cout << "Token "<< tokens[i]->getTypeString() << ", value: "<< tokens[i]->getToken() << std::endl;
    }
}
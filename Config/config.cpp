#include "config.hpp"
Token::Token(std::string token, TokenType tokentype) : _Token(token), _tokenType(tokentype)
{
    std::cout << "Token creation\n";
}
Token::~Token() {}
TokenType Token::getType() {return(_tokenType);}
std::string Token::getToken() {return (_Token);}

std::string Token::getTypeString() {
    static const char* typeNames[] = {"ident", "openbracket", "closebracket", "semicolon", "string"};
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
            tokens.push_back(new Token(line, ident));
            line.clear();
            while(isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == ';')
        {
            if (!line.empty())
                tokens.push_back(new Token(line, ident));
            line.clear();
            //std::cout << ch << "  ||  " << line << std::endl;
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
        //std::cout << ch << "  ||  " << line << std::endl;
        if (ch != ';' && ch != '\n' && ch != '#' && ch!= '\'' && !file.eof())
            line += ch;
    }
    if (!line.empty())
        tokens.push_back(new Token(line, ident));
    for (size_t i = 0; i < tokens.size(); i++)
    {
        std::cout << "Type : "<< tokens[i]->getTypeString() << ", Value : "<< tokens[i]->getToken() << std::endl;
    }
}
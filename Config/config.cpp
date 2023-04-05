#include "config.hpp"
Token::Token(std::string token, type tokentype) : _Token(token), _tokenType(tokentype)
{
    std::cout << "Token creation\n";
}
Token::~Token() {}
std::string Token::getType() {
    static const char* typeNames[] = {"ident", "openbracket", "closebracket", "semicolon", "string"};
    return typeNames[_tokenType];
}
std::string Token::getToken() {return (_Token);}

int main()
{
    char ch;
    std::vector<Token *> tokens;
    std::string line;
    std::string filename = "../config.conf";
    std::ifstream file(filename.c_str());

    while (file.get(ch))
    {
        std::cout << ch << std::endl;
        if (ch == ';')
        {
            if (line.size() > 1)
            {
                file.unget();
                std::cout << "1";
                tokens.push_back(new Token(line, ident));
                line.clear();
            }
            else
            {
                std::cout << "2";
                tokens.push_back(new Token(";", semicolon));
                file.get();
                line.clear();
            }
        }
        else if (isspace(ch))
        {
            std::cout << "3";
            tokens.push_back(new Token(line, ident));
            line.clear();
            while (isspace(ch))
                file.get(ch);
        }
        if (ch != ';')
            line += ch;
    }
    for (size_t i = 0; i < tokens.size(); i++)
    {
        std::cout << "Type : "<< tokens[i]->getType() << ", Value : "<< tokens[i]->getToken() << std::endl;
    }
}
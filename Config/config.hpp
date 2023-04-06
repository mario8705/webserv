#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <fstream>


enum TokenType {
    ident,
    openbracket,
    closebracket,
    semicolon,
    string,
};
class Token {
public:
    Token(std::string token, TokenType tokentype);
    ~Token();
    TokenType getType();
    std::string getTypeString();
    std::string getToken();


private:
    TokenType _tokenType;
    std::string _Token;
};
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


enum type {
    ident,
    openbracket,
    closebracket,
    semicolon,
    string,
};
class Token {
public:
    Token(std::string token, type tokentype);
    ~Token();
    std::string getType();
    std::string getToken();


private:
    type _tokenType;
    std::string _Token;
};
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
#include <stack>


enum TokenType {
    kTokenType_Ident,
    kTokenType_Leftbracket,
    kTokenType_Rightbracket,
    kTokenType_Semicolon,
    kTokenType_String,
};


class Token {
public:
    Token(std::string token, TokenType tokentype);
    ~Token();
    TokenType getType();
    std::string getTypeString();
    std::string getToken();
    static void tokenization(std::vector<Token *>*tokens);



private:
    TokenType _tokenType;
    std::string _Token;
};
class ConfigProperty
{
public:
    ConfigProperty(std::vector<std::string>Tokens);
    ~ConfigProperty();
    static void push_config(std::vector<Token *> tokens, std::vector<ConfigProperty *> *config);
    void add_body(ConfigProperty *value);
    const std::vector<ConfigProperty *> &getBodystring();
    std::vector<std::string> getParams();
private:
    std::vector<std::string> _params;
    std::vector<ConfigProperty *> _body;

};

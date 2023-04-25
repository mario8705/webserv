#include "config.hpp"

/*          ----- TOKEN CLASS ----              */
Token::Token(std::string token, TokenType tokentype) : _Token(token), _tokenType(tokentype) {}
Token::~Token() {}
TokenType Token::getType() {return(_tokenType);}
std::string Token::getToken() {return (_Token);}

std::string Token::getTypeString() {
    static const char* typeNames[] = {"ident", "left bracket", "right bracket", "semicolon", "string"};
    return typeNames[_tokenType];
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
            tokens->push_back(new Token("{", kTokenType_Leftbracket));
            line.clear();
            while(isspace(ch) && !file.eof())
                file.get(ch);
        }
        if (ch == '}')
        {
            tokens->push_back(new Token("}", kTokenType_Rightbracket));
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

/*          ----- CONFIG CLASS ----              */

ConfigProperty::ConfigProperty(std::vector<std::string> Tokens) : _params(Tokens) {}
ConfigProperty::~ConfigProperty() {}

std::vector<std::string> ConfigProperty::getParams() {
    return(_params);
}

void ConfigProperty::add_body(ConfigProperty *value) {
    _body.push_back(value);
}

const std::vector<ConfigProperty *> &ConfigProperty::getBodystring() {
    return (_body);
}

void ConfigProperty::push_config(std::vector<Token *> tokens, std::vector<ConfigProperty *> *config) {
    std::vector<std::string> tmp;
    std::stack<ConfigProperty *> stck;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i]->getTypeString() != "left bracket" && tokens[i]->getTypeString() != "right bracket")
            tmp.push_back(tokens[i]->getToken());
        if (tokens[i]->getTypeString() == "left bracket") {
            stck.push(new ConfigProperty(tmp));
            config->push_back(stck.top());
            tmp.clear();
        }
        else if (tokens[i]->getTypeString() == "right bracket")
            stck.pop();
        if (tokens[i]->getTypeString() == "semicolon") {
            if (!stck.empty()) {
                stck.top()->add_body(new ConfigProperty(tmp));
            } else
                config->push_back(new ConfigProperty(tmp));
            tmp.clear();
        }
    }
}

int main()
{
    try {
        std::vector<Token *> tokens;
        std::vector<ConfigProperty *> configs;
        Token::tokenization(&tokens);
        ConfigProperty::push_config(tokens, &configs);
        /*std::cout << "---------------\nCONFIGS :: \n\n";
        for (size_t i = 0; i < configs.size(); i++) {
            std::vector<std::string> tmp = configs[i]->getParams();
            std::vector<ConfigProperty *> tmp2 = configs[i]->getBodystring();
            for (size_t j = 0; j < tmp.size(); j++) {
                std::cout << "CONFIG :: " << tmp[j] << std::endl;
                for (size_t k = 0; k < tmp2.size(); k++) {
                    std::vector<std::string> tmp3 = tmp2[k]->getParams();
                    std::cout << "Rule of :: " << tmp[j] << " :: ";
                    for (size_t l = 0; l < tmp3.size(); l++) {
                        std::cout << tmp3[l];
                    }
                    std::cout << std::endl;
                }
            }
        }*/
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}
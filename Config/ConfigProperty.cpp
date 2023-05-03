#include "ConfigProperty.h"
#include <stack>
#include "Token.h"

ConfigProperty::ConfigProperty(const std::vector<std::string> &tokens)
    : _params(tokens)
{
}

ConfigProperty::~ConfigProperty()
{
}

const std::vector<std::string> &ConfigProperty::getParams() const
{
    return _params;
}

void ConfigProperty::add_body(ConfigProperty *value)
{
    _body.push_back(value);
}

const std::vector<ConfigProperty *> &ConfigProperty::getBody() const
{
    return (_body);
}

void ConfigProperty::push_config(std::vector<Token *> tokens, std::vector<ConfigProperty *> *config)
{
    std::vector<std::string> tmp;
    std::stack<ConfigProperty *> stck;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i]->getType() != kTokenType_LeftBracket && tokens[i]->getType() != kTokenType_RightBracket)
            tmp.push_back(tokens[i]->getToken());
        if (tokens[i]->getType() == kTokenType_LeftBracket) {
            stck.push(new ConfigProperty(tmp));
            config->push_back(stck.top());
            tmp.clear();
        }
        else if (tokens[i]->getType() == kTokenType_LeftBracket)
            stck.pop();
        if (tokens[i]->getType() == kTokenType_Semicolon) {
            if (!stck.empty()) {
                stck.top()->add_body(new ConfigProperty(tmp));
            } else
                config->push_back(new ConfigProperty(tmp));
            tmp.clear();
        }
    }
}
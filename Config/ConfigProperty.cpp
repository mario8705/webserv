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

void ConfigProperty::push_config(const std::vector<Token *> &tokens, std::vector<ConfigProperty *> &config)
{
    std::vector<std::string> tmp;
    std::stack<ConfigProperty *> stck;
    ConfigProperty *property;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i]->getType() == kTokenType_Ident ||
            tokens[i]->getType() == kTokenType_String)
        {
            tmp.push_back(tokens[i]->getToken());
        }
        else if (tokens[i]->getType() == kTokenType_LeftBracket)
        {
            property = new ConfigProperty(tmp);
            if (stck.empty())
                config.push_back(property);
            else
                stck.top()->add_body(property);
            stck.push(property);
            tmp.clear();
        }
        else if (tokens[i]->getType() == kTokenType_RightBracket)
        {
            stck.pop();
        }
        else if (tokens[i]->getType() == kTokenType_Semicolon)
        {
            if (!tmp.empty())
            {
                if (stck.empty())
                    config.push_back(new ConfigProperty(tmp));
                else
                    stck.top()->add_body(new ConfigProperty(tmp));
                tmp.clear();
            }
        }
    }
}

bool ConfigProperty::IsBlockSection(const std::string &name) const
{
    return (!_params.empty() && name == _params[0]);
}
#include "ConfigProperty.h"
#include <stack>
#include "Token.h"

ConfigProperty::ConfigProperty()
{
}

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

ConfigProperty *ConfigProperty::push_config(const std::vector<Token *> &tokens)
{
    std::vector<std::string> tmp;
    std::stack<ConfigProperty *> stck;
    ConfigProperty *property;

    stck.push(new ConfigProperty);
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
            stck.top()->add_body(property);
            stck.push(property);
            tmp.clear();
        }
        else if (tokens[i]->getType() == kTokenType_RightBracket)
        {
            if (stck.size() < 2)
            {
                /* TODO error */
            }
            stck.pop();
        }
        else if (tokens[i]->getType() == kTokenType_Semicolon)
        {
            if (!tmp.empty())
            {
                stck.top()->add_body(new ConfigProperty(tmp));
                tmp.clear();
            }
        }
    }
    return stck.top();
}

const std::string &ConfigProperty::GetName() const
{
    return _params[0];
}

bool ConfigProperty::IsBlock() const
{
    /* TODO http {} is considered a property and not a block, change this ? */
    return !_body.empty();
}
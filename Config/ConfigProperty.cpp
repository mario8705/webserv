#include "ConfigProperty.h"
#include <stack>
#include "Token.h"

ConfigProperty::ConfigProperty()
    : m_isBlock(true)
{
}

ConfigProperty::ConfigProperty(const std::vector<std::string> &params, bool isBlock)
    : _params(params), m_isBlock(isBlock)
{
}

ConfigProperty::~ConfigProperty()
{
    size_t i;

    for (i = 0; i < _body.size(); ++i)
    {
        delete _body[i];
    }
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
            property = new ConfigProperty(tmp, true);
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
                stck.top()->add_body(new ConfigProperty(tmp, false));
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
    return m_isBlock;
}

void ConfigProperty::Dump(std::ostream &out, int indent)
{
    Dump(out, this, 0, indent);
    out.flush();
}

static inline void spaces(std::ostream &out, int n)
{
    while (n--)
        out.put(' ');
}

void ConfigProperty::Dump(std::ostream &out, ConfigProperty *prop, int depth, int indent)
{
    size_t i;

    if (depth >= 16)
        return ;
    spaces(out, depth * indent);
    for (i = 0; i < prop->_params.size(); ++i) {
        if (i > 0)
            out << " ";
        out << prop->_params[i];
    }
    if (prop->IsBlock())
    {
        if (indent > 0)
            out << " {\n";
        else
            out << "{";
        for (i = 0; i < prop->_body.size(); ++i)
        {
            Dump(out, prop->_body[i], depth + 1, indent);
        }
        spaces(out, depth * indent);
        out << "}";
        if (indent > 0)
            out << "\n";
    }
    else
    {
        out << ";";
        if (indent > 0)
            out << "\n";
    }
}

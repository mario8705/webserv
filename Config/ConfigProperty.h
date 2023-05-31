#ifndef WEBSERV_CONFIGPROPERTY_H
#define WEBSERV_CONFIGPROPERTY_H
#include <vector>
#include <string>
#include "../string_utils.hpp"

class Token;
class ConfigProperty;

class PropertyIterator
{
    friend class ConfigProperty;

public:
    ~PropertyIterator()
    {
    }

    ConfigProperty *Next();

private:
    PropertyIterator(const std::vector<ConfigProperty *> &v, const std::string &filter, bool blockOnly)
        : m_filter(filter), m_current(v.begin()), m_end(v.end()), m_blockOnly(blockOnly)
    {
    }

    std::string m_filter;
    bool m_blockOnly;
    std::vector<ConfigProperty *>::const_iterator m_current;
    std::vector<ConfigProperty *>::const_iterator m_end;
};

class ConfigProperty
{
public:
    ConfigProperty();
    ConfigProperty(const std::vector<std::string> &params, bool isBlock);
    ~ConfigProperty();

    void add_body(ConfigProperty *value);

    const std::vector<ConfigProperty *> &getBody() const;
    const std::vector<std::string> &getParams() const;

    const std::string &GetName() const;
    bool IsBlock() const;

    void Dump(std::ostream &out, int indent = 0);

    PropertyIterator FindAllProps(const std::string &name = "") const
    {
        return PropertyIterator(_body, name, false);
    }

    PropertyIterator FindAllBlocks(const std::string &name = "") const
    {
        return PropertyIterator(_body, name, true);
    }

    size_t GetParamsCount() const
    {
        return _params.size();
    }

    static ConfigProperty *push_config(ConfigProperty *rootProp, const std::vector<Token *> &tokens);

private:
    std::vector<std::string> _params;
    std::vector<ConfigProperty *> _body;
    bool m_isBlock;

    static void Dump(std::ostream &out, ConfigProperty *prop, int depth, int indent);
};


#endif //WEBSERV_CONFIGPROPERTY_H

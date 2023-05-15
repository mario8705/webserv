#ifndef WEBSERV_CONFIGPROPERTY_H
#define WEBSERV_CONFIGPROPERTY_H
#include <vector>
#include <string>

class Token;

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

    static ConfigProperty *push_config(const std::vector<Token *> &tokens);

private:
    std::vector<std::string> _params;
    std::vector<ConfigProperty *> _body;
    bool m_isBlock;

    static void Dump(std::ostream &out, ConfigProperty *prop, int depth, int indent);
};


#endif //WEBSERV_CONFIGPROPERTY_H

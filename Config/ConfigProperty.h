#ifndef WEBSERV_CONFIGPROPERTY_H
#define WEBSERV_CONFIGPROPERTY_H
#include <vector>
#include <string>

class Token;

class ConfigProperty
{
public:
    ConfigProperty();
    explicit ConfigProperty(const std::vector<std::string> &tokens);
    ~ConfigProperty();

    void add_body(ConfigProperty *value);

    const std::vector<ConfigProperty *> &getBody() const;
    const std::vector<std::string> &getParams() const;

    const std::string &GetName() const;
    bool IsBlock() const;

    static ConfigProperty *push_config(const std::vector<Token *> &tokens);

private:
    std::vector<std::string> _params;
    std::vector<ConfigProperty *> _body;
};


#endif //WEBSERV_CONFIGPROPERTY_H

#ifndef WEBSERV_CONFIGPROPERTY_H
#define WEBSERV_CONFIGPROPERTY_H
#include <vector>
#include <string>

class Token;

class ConfigProperty
{
public:
    explicit ConfigProperty(const std::vector<std::string> &tokens);
    ~ConfigProperty();

    void add_body(ConfigProperty *value);

    const std::vector<ConfigProperty *> &getBody() const;
    const std::vector<std::string> &getParams() const;

    static void push_config(std::vector<Token *> tokens, std::vector<ConfigProperty *> *config);

private:
    std::vector<std::string> _params;
    std::vector<ConfigProperty *> _body;
};


#endif //WEBSERV_CONFIGPROPERTY_H

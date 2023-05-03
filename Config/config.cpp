#include "ConfigProperty.h"
#include "Token.h"
#include <iostream>

int main()
{
    try {
        std::vector<Token *> tokens;
        std::vector<ConfigProperty *> configs;
        Token::tokenization(&tokens);
        ConfigProperty::push_config(tokens, &configs);

        std::cout << "---------------\nCONFIGS :: \n\n";
        for (size_t i = 0; i < configs.size(); i++) {
            std::vector<std::string> tmp = configs[i]->getParams();
            std::vector<ConfigProperty *> tmp2 = configs[i]->getBody();
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
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}
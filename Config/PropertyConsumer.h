//
// Created by Alexis Lavaud on 08/05/2023.
//

#ifndef WEBSERV_PROPERTYCONSUMER_H
#define WEBSERV_PROPERTYCONSUMER_H
#include <vector>
#include <iostream>
#include "ConfigProperty.h"

class PropertyConsumer
{
public:
    explicit PropertyConsumer(ConfigProperty *property);

    ~PropertyConsumer();

    template <typename T, typename Function>
    void AcceptBlocks(const std::string &blockName, Function cb, T *thisPtr) {
        std::vector<ConfigProperty *>::iterator it;
        ConfigProperty *property;

        for (it = m_blocks.begin(); it != m_blocks.end();) {
            property = *it;
            if (property->GetName() == blockName) {
                if (!property->IsBlock()) {
                    std::cout << "Warning: expected a block but found a property when parsing "
                              << property->GetName() << std::endl;
                } else {
                    (thisPtr->*cb)(property);
                }
                it = m_blocks.erase(it);
            } else {
                ++it;
            }
        }
    }

    template <typename T, typename Function, typename Validator>
    void AcceptProperties(const std::string &blockName, Function cb, T *thisPtr, Validator validator)
    {
        std::vector<ConfigProperty *>::iterator it;
        ConfigProperty *property;

        for (it = m_blocks.begin(); it != m_blocks.end(); )
        {
            property = *it;
            if (blockName.empty() || property->GetName() == blockName)
            {
                if (property->IsBlock())
                {
                    std::cout << "Warning: expected a property but found a block when parsing "
                              << property->GetName() << std::endl;
                }
                else {
                    if (validator(property)) {
                        (thisPtr->*cb)(property);
                    }
                }
                it = m_blocks.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

protected:
    ConfigProperty *m_property;
    std::vector<ConfigProperty *> m_blocks;
};

#endif //WEBSERV_PROPERTYCONSUMER_H

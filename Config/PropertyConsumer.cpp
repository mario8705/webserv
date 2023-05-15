//
// Created by Alexis Lavaud on 08/05/2023.
//
#include "PropertyConsumer.h"

PropertyConsumer::PropertyConsumer(ConfigProperty *property)
        : m_property(property)
{
    m_blocks = property->getBody();
}

PropertyConsumer::~PropertyConsumer()
{
    std::vector<ConfigProperty *>::iterator it;
    ConfigProperty *property;

    for (it = m_blocks.begin(); it != m_blocks.end(); ++it)
    {
        property = *it;
        if (property->IsBlock())
        {
            std::cerr << "Unknown block " << property->getParams()[0] << std::endl;
        }
        else
        {
            std::cerr << "Unknown property " << property->getParams()[0] << std::endl;
        }
    }
}
//
// Created by alavaud on 5/19/23.
//
#include "ConfigProperty.h"
#include "../string_utils.hpp"

bool PropertyIterator::HasNext() const
{
    return (m_current != m_end);
}

ConfigProperty *PropertyIterator::Next()
{
    ConfigProperty *prop;

    while (m_current != m_end)
    {
        prop = *m_current++;
        if (prop->IsBlock() != m_blockOnly)
            continue;
        if (prop->GetName() == m_filter)
        {
            return prop;
        }
    }
    return NULL;
}

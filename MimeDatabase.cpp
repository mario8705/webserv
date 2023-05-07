//
// Created by Alexis Lavaud on 07/05/2023.
//

#include "MimeDatabase.h"

MimeDatabase::MimeDatabase(const std::string &defaultType)
    : m_defaultType(defaultType)
{
}

MimeDatabase::~MimeDatabase()
{
}

std::string MimeDatabase::GetMimeType(const std::string &filename) const
{
    size_t pos;
    tMimeTypesDatabase::const_iterator it;

    if (std::string::npos != (pos = filename.find_last_of('.')))
    {
        if (m_database.end() != (it = m_database.find(filename.substr(pos + 1))))
        {
            return it->second;
        }
    }
    return m_defaultType;
}

bool MimeDatabase::RegisterType(const std::string &ext, const std::string &mime)
{
    if (m_database.find(ext) != m_database.end())
        return false;
    m_database.insert(std::make_pair(ext, mime));
    return true;
}

void MimeDatabase::SetDefaultType(const std::string &defaultType)
{
    m_defaultType = defaultType;
}

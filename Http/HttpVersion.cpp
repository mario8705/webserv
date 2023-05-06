//
// Created by Alexis Lavaud on 05/05/2023.
//
#include "HttpVersion.h"

HttpVersion::HttpVersion()
    : m_major(1), m_minor(0)
{
}

HttpVersion::HttpVersion(int major, int minor)
    : m_major(major), m_minor(minor)
{
}

HttpVersion::~HttpVersion()
{
}

bool HttpVersion::IsCompatible(const HttpVersion &version) const
{
    return false; /* TODO */
}

int HttpVersion::GetMajor() const
{
    return m_major;
}

int HttpVersion::GetMinor() const
{
    return m_minor;
}

bool HttpVersion::Parse(const std::string &version) {
    if (version.compare(0, 5, "HTTP/") != 0 || version.size() != 8)
        return false;
    if (!std::isdigit(version[7]) || !std::isdigit(version[5]))
        return false;
    if ('.' != version[6])
        return false;
    m_major = (version[5] - '0');
    m_minor = (version[7] - '0');
    return true;
}

HttpVersion HttpVersion::kHttpVersion_1_0 = HttpVersion(1, 0);
HttpVersion HttpVersion::kHttpVersion_1_1 = HttpVersion(1, 1);

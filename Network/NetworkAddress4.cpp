//
// Created by Alexis Lavaud on 16/03/2023.
//

#include "NetworkAddress4.h"

NetworkAddress4::NetworkAddress4(uint16_t port)
    : m_address(0), m_port(port)
{
}

NetworkAddress4::NetworkAddress4(uint32_t address, uint16_t port)
    : m_address(address), m_port(port)
{
}

NetworkAddress4::NetworkAddress4(const NetworkAddress4 &other)
    : m_address(other.m_address), m_port(other.m_port)
{
}

NetworkAddress4::~NetworkAddress4()
{
}

NetworkAddress4 &NetworkAddress4::operator=(const NetworkAddress4 &other)
{
    m_address = other.m_address;
    m_port = other.m_port;
    return *this;
}

bool NetworkAddress4::operator==(const NetworkAddress4 &other) const
{
    return m_address == other.m_address && m_port == other.m_port;
}

uint32_t NetworkAddress4::GetAddress() const
{
    return m_address;
}

uint16_t NetworkAddress4::GetPort() const
{
    return m_port;
}

static bool ExtractByte(int &b, const char *s, size_t len)
{
    size_t i;

    if (len < 1 || len > 3)
        return false;
    for (i = 0, b = 0; i < len; ++i)
    {
        if (!std::isdigit(s[i]))
            return false;
        b = b * 10 + (s[0] - '0');
    }
    return (b < 255);
}

NetworkAddress4 NetworkAddress4::Parse(const std::string &address, uint16_t port) {
    uint32_t addr;
    size_t sep, sep2;
    int a[4];

    addr = 0;
    sep = address.find('.');
    if (std::string::npos == sep || !ExtractByte(a[0], address.c_str(), sep))
        throw; /* TODO */
    sep2 = address.find('.', sep + 1);
    if (std::string::npos == sep2 || !ExtractByte(a[1], address.c_str() + sep + 1, sep2 - sep - 1))
        throw;

    return NetworkAddress4(addr, port);
}

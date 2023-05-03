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

NetworkAddress4 NetworkAddress4::Parse(const std::string &address, uint16_t port) {
    uint32_t addr;

    addr = 0; /* TODO */

    return NetworkAddress4(addr, port);
}

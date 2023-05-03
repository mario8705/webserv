//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_NETWORKADDRESS4_H
#define WEBSERV_NETWORKADDRESS4_H
#include <string>
#include <stdint.h>

/**
 * Holds an address/port pair in IPv4 format.
 * Suitable for TCP and UDP.
 */
class NetworkAddress4
{
public:
    /**
     * Constructs a network address with the specified port and 0.0.0.0 as the IP address
     * @param port The port of the address
     */
    explicit NetworkAddress4(uint16_t port);

    /**
     * Constructs a network address with the specified IP address and port
     * @param address The IPv4 address in host byte order (127.0.0.1 is 0x7F000001)
     * @param port The port in host byte order
     */
    NetworkAddress4(uint32_t address, uint16_t port);

    NetworkAddress4(const NetworkAddress4 &);
    ~NetworkAddress4();

    NetworkAddress4 &operator=(const NetworkAddress4 &);

    bool operator==(const NetworkAddress4 &) const;

    uint32_t GetAddress() const;
    uint16_t GetPort() const;

    static NetworkAddress4 Parse(const std::string &address, uint16_t port);

private:
    uint32_t m_address;
    uint16_t m_port;
};


#endif //WEBSERV_NETWORKADDRESS4_H

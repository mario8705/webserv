//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_NETWORKADDRESS4_H
#define WEBSERV_NETWORKADDRESS4_H
#include <cstdint>

class NetworkAddress4
{
public:
    explicit NetworkAddress4(uint16_t port);
    NetworkAddress4(const NetworkAddress4 &);
    ~NetworkAddress4();

    NetworkAddress4 &operator=(const NetworkAddress4 &);

    bool operator==(const NetworkAddress4 &) const;

    uint32_t GetAddress() const;
    uint16_t GetPort() const;

private:
    uint32_t m_address;
    uint16_t m_port;
};


#endif //WEBSERV_NETWORKADDRESS4_H

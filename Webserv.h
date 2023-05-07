//
// Created by Alexis Lavaud on 16/03/2023.
//

#ifndef WEBSERV_WEBSERV_H
#define WEBSERV_WEBSERV_H
#include <csignal>
#include <string>
#include <vector>
#include <map>
#include "Network/NetworkAddress4.h"

class IEventLoop;
class ListenerEvent;
class ServerHost;
class VirtualHost;
class ConfigProperty;

namespace ft
{
    template< class InputIt1, class InputIt2, class Compare >
    bool lexicographical_compare( InputIt1 first1, InputIt1 last1,
                                  InputIt2 first2, InputIt2 last2, Compare comp )
    {
        for (; (first1 != last1) && (first2 != last2); ++first1, (void) ++first2)
        {
            if (comp(*first1, *first2))
                return true;
            if (comp(*first2, *first1))
                return false;
        }
        return (first1 == last1) && (first2 != last2);
    }
}

struct case_insensitive_less
{
    struct case_insensitive_compare
    {
        bool operator()(const unsigned char &c1, const unsigned char &c2) const
        {
            return std::tolower(c1) < std::tolower(c2);
        }
    };

    bool operator()(const std::string &lhs, const std::string &rhs) const
    {
        return ft::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                case_insensitive_compare()
                );
    }
};

class Webserv
{
public:
    typedef std::vector<ServerHost *> tHostList;
    typedef std::vector<VirtualHost *> tVirtualHostList;
    typedef std::map<std::string, std::string, case_insensitive_less> tMimeTypesDatabase;

    Webserv();
    ~Webserv();

    bool CreateServer(NetworkAddress4 addr);

    bool LoadConfig(const std::string &path);

    void Run();

    bool IsRunning() const;
    void Stop();

private:
    IEventLoop *m_eventLoop;
    std::vector<ListenerEvent *> m_listeners;
    tHostList m_hosts;
    tVirtualHostList m_virtualHosts;
//    std::map<NetworkAddress4, ServerHost *> m_hosts;
    volatile sig_atomic_t m_running;
    tMimeTypesDatabase m_mimeTypes;
    std::string m_defaultMimeType;

    void ParseConfig(ConfigProperty *rootBlock);
    void ParseHttpBlock(ConfigProperty *httpBlock);
    void ParseTypesBlock(ConfigProperty *typesBlock);

    void ParseMimeType(ConfigProperty *mime);
};


#endif //WEBSERV_WEBSERV_H

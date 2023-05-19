//
// Created by Alexis Lavaud on 04/04/2023.
//

#ifndef WEBSERV_HTTPVERSION_H
#define WEBSERV_HTTPVERSION_H
#include <string>
#include <ostream>

class HttpVersion
{
public:
    HttpVersion();
    HttpVersion(int major, int minor);
    ~HttpVersion();

    bool IsCompatible(const HttpVersion &version) const;

    int GetMajor() const;
    int GetMinor() const;

    bool Parse(const std::string &version);

    std::string ToString() const;

    static HttpVersion kHttpVersion_1_0;
    static HttpVersion kHttpVersion_1_1;

private:
    int m_major;
    int m_minor;
};

inline std::ostream &operator<<(std::ostream &os, const HttpVersion &version)
{
    os << "HTTP/" << version.GetMajor() << "." << version.GetMinor();
    return os;
}

/* TOOD Temporary */
inline bool operator==(const HttpVersion &a, const HttpVersion &b)
{
    return a.GetMajor() == b.GetMajor() && a.GetMinor() == b.GetMinor();
}

#endif //WEBSERV_HTTPVERSION_H

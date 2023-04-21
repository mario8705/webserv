//
// Created by Alexis Lavaud on 04/04/2023.
//

#include "URL.h"

URL::URL() {

}

URL::~URL() {

}

URL URL::ParseURL(const std::string &url) {
    //printf("Parsing: %s\n", url.c_str());

    return URL();
}

static int hexval(int ch)
{
    if (ch >= '0' && ch <= '9')
        return (ch - '0');
    else if (ch >= 'a' && ch <= 'f')
        return (ch - 'a');
    else if (ch >= 'A' && ch <= 'F')
        return (ch - 'A');
    return -1;
}

std::string URL::Decode(const std::string &s) {
    std::string res;
    size_t i;
    int ch;
    int val[2];

    for (i = 0; i < s.size(); )
    {
        ch = (unsigned char)s[i++];
        if (ch == '%' &&
            (i + 2) < s.size() &&
            (val[0] = hexval(s[i + 0])) >= 0 &&
            (val[1] = hexval(s[i + 1])) >= 0)
        {
            ch = (val[0] << 4) | val[1];
            i += 2;
        }
        res.insert(res.end(), static_cast<char>(ch));
    }
 //   printf("Decoded '%s' into '%s'\n", s.c_str(), res.c_str());
    return res;
}

std::string URL::Encode(const std::string &s)
{
    std::string res;
    size_t i;
    int ch;

    for (i = 0; i < s.size(); ++i)
    {
        ch = (unsigned char)s[i];

        res.insert(res.end(), static_cast<char>(ch));
    }
    return res;
}

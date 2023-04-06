//
// Created by Alexis Lavaud on 15/03/2023.
//

#ifndef WEBSERV_BUFFERCHAIN_H
#define WEBSERV_BUFFERCHAIN_H
#include <cstdlib>

class BufferChain
{
public:
    BufferChain();
    ~BufferChain();

    size_t GetFreeSpace() const;

    static BufferChain *Allocate(size_t capacity);

//private:
    char *m_buffer;
    size_t m_misalign;
    size_t m_offset;
    size_t m_size;

private:
};


#endif //WEBSERV_BUFFERCHAIN_H

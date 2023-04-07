//
// Created by Alexis Lavaud on 15/03/2023.
//

#ifndef WEBSERV_BUFFERCHAIN_H
#define WEBSERV_BUFFERCHAIN_H
#include <cstdlib>
#include <vector>

class BufferChain
{
public:
    BufferChain();
    ~BufferChain();

    size_t GetFreeSpace() const;

    void Release();

    static BufferChain *Allocate(size_t capacity);
    static void ReleasePool();

    static const int kMaxPooledChains;

//private:
    char *m_buffer;
    size_t m_misalign;
    size_t m_offset;
    size_t m_size;

private:
    static std::vector<BufferChain *> m_pool;
};


#endif //WEBSERV_BUFFERCHAIN_H

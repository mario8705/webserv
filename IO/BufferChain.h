//
// Created by Alexis Lavaud on 15/03/2023.
//

#ifndef WEBSERV_BUFFERCHAIN_H
#define WEBSERV_BUFFERCHAIN_H
#include <cstdlib>

typedef enum {
    kSegmentType_Memory,
    kSegmentType_File,
} eBufferSegmentType;

class BufferChain
{
public:
    BufferChain();
    ~BufferChain();

    size_t GetFreeSpace() const
    {
        return m_size - m_offset;
    }

//private:
    eBufferSegmentType m_type;
    char *m_buffer;
    int m_fd;
    size_t m_misalign;
    size_t m_offset;
    size_t m_size;
};


#endif //WEBSERV_BUFFERCHAIN_H

//
// Created by Alexis Lavaud on 15/03/2023.
//

#ifndef WEBSERV_DATABUFFER_H
#define WEBSERV_DATABUFFER_H
#include <cstdlib>
#include <string>
#include <vector>

class BufferChain;

class BufferPtr
{
public:

    /* Offset into the DataBuffer */
    size_t m_offset;

    /* Internal */
    BufferChain *m_chain;
    size_t m_chainOffset;
};

class DataBuffer
{
public:
    typedef std::vector<BufferChain *> tChainList;

    DataBuffer();
    ~DataBuffer();

    /**
     * Writes n bytes of data into the buffer.
     * @param data The pointer to the start of the data to write
     * @param n The amount of bytes to be written into the buffer
     * @return The number of bytes actually written into the buffer.
     */
    int Write(const void *data, size_t n);

    void AddBuffer(DataBuffer *buffer);

    int PutString(const std::string &s);

    int AddFile(int fd, size_t offset, size_t length);

    int CopyOut(void *data, size_t n);

    int FindEOL(BufferPtr &ptr) const;

    bool Readln(std::string &line);

    /**
     * Receive data from fd and put it into the buffer.
     * @param fd
     * @return The number of bytes received.
     */
    int Receive(int fd);

    int Send(int fd);

    size_t GetLength() const { return m_length; }

private:
    BufferChain *NewMemorySegment(size_t minCapacity);

    size_t m_length;
    tChainList m_chains;
    tChainList m_freeChains;
};


#endif //WEBSERV_DATABUFFER_H

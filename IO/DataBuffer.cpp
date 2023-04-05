//
// Created by Alexis Lavaud on 15/03/2023.
//

#include "DataBuffer.h"
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include "BufferChain.h"

DataBuffer::DataBuffer()
{
    m_length = 0;
    m_readHighWatermark = 0xFFFFFFFF;
}

DataBuffer::~DataBuffer()
{
    tChainList::iterator it;

    for (it = m_chains.begin(); it != m_chains.end(); ++it)
    {
        delete *it;
    }
    for (it = m_freeChains.begin(); it != m_freeChains.end(); ++it)
    {
        delete *it;
    }
}

int DataBuffer::Write(const void *data, size_t n)
{
    BufferChain *chain;
    size_t bufsize;

    /* Check if the last chain contains enough space */
    chain = NULL;
    if (!m_chains.empty())
    {
        chain = m_chains[m_chains.size() - 1];
        if (chain->GetFreeSpace() < n)
        {
            chain = NULL;
        }
    }
    /* Not enough free space/empty chain list */
    if (!chain)
    {
        bufsize = n;
        if (bufsize < 4096)
            bufsize = 4096;
        chain = NewMemorySegment(bufsize);
    }
    memcpy(chain->m_buffer + chain->m_offset, data, n);
    chain->m_offset += n;
    m_length += n;
    return n;
}

int DataBuffer::PutString(const std::string &s)
{
    return Write(s.c_str(), s.size());
}

bool DataBuffer::Readln(std::string &line) {
    BufferPtr ptr;

    if (!FindEOL(ptr))
        return false;
    line.resize(ptr.m_offset);
    /* TODO Find a better way to do this */
    CopyOut((char *) line.c_str(), ptr.m_offset);
    if (line.size() > 0 && line[line.size() - 1] == '\n')
        line.resize(line.size() - 1);
    if (line.size() > 0 && line[line.size() - 1] == '\r')
        line.resize(line.size() - 1);
    return true;
}

int DataBuffer::Receive(int fd)
{
    BufferChain *chain;
    ssize_t n;
    size_t avail;

    chain = NULL;
    if (m_chains.size() > 0)
    {
        chain = m_chains[m_chains.size() - 1];
        if (chain->GetFreeSpace() == 0)
            chain = NULL;
    }
    if (!chain)
        chain = NewMemorySegment(4096);
    avail = m_readHighWatermark - m_length;
    if (avail > chain->GetFreeSpace())
        avail = chain->GetFreeSpace();
    n = recv(fd, chain->m_buffer + chain->m_offset, avail, 0);
    printf("Chain : %lu\n", avail);
    if (n > 0) {
        chain->m_offset += n;
        m_length += n;
    }
    return static_cast<int>(n);
}

BufferChain *DataBuffer::NewMemorySegment(size_t minCapacity)
{
    std::vector<BufferChain *>::iterator it;
    std::vector<BufferChain *>::iterator chainIt;
    BufferChain *chain;
    size_t closestCap;

    closestCap = SIZE_MAX;
    chainIt = m_freeChains.end();
    for (it = m_freeChains.begin(); it != m_freeChains.end(); ++it)
    {
        chain = *it;
        if (chain->m_type == kSegmentType_Memory &&
        chain->m_size >= minCapacity &&
        chain->m_size < closestCap)
        {
            closestCap = chain->m_size;
            chainIt = it;
        }
    }
    if (chainIt != m_freeChains.end())
    {
        chain = *chainIt;
        m_freeChains.erase(chainIt);
        chain->m_offset = 0;
        chain->m_misalign = 0;
        printf("Recycled chain\n");
    }
    else
    {
        chain = new BufferChain;
        chain->m_type = kSegmentType_Memory;
        chain->m_size = minCapacity;
        chain->m_misalign = 0;
        chain->m_offset = 0;
        chain->m_buffer = new char[minCapacity];
        printf("Allocated new chain\n");
    }
    m_chains.push_back(chain);
    return chain;
}

int DataBuffer::FindEOL(BufferPtr &ptr) const
{
    std::vector<BufferChain *>::const_iterator it;
    BufferChain *chain;
    size_t i;
    size_t off;
    bool nl;

    off = 0;
    nl = false;
    for (it = m_chains.cbegin(); it != m_chains.cend() && !nl; ++it)
    {
        chain = *it;

        /* XXX: Shouldn't attempt to read a file */
        if (chain->m_type == kSegmentType_Memory)
        {
            for (i = chain->m_misalign; i < chain->m_offset && !nl; ++i, ++off)
            {
                nl = (chain->m_buffer[i] == '\n');
            }
        }
    }
    if (nl) {
        ptr.m_offset = off;
        ptr.m_chain = chain;
        ptr.m_chainOffset = i;
    }
    return nl;
}

int DataBuffer::CopyOut(void *data, size_t n)
{
    tChainList::iterator it;
    BufferChain *chain;
    size_t sz;
    size_t total;
    char *ptr;

    ptr = (char *)data;
    total = 0;
    for (it = m_chains.begin(); it != m_chains.end() && total < n; )
    {
        chain = *it;
        sz = chain->m_offset - chain->m_misalign;
        if (sz > n)
            sz = n;
        memcpy(ptr + total, chain->m_buffer + chain->m_misalign, sz);
        chain->m_misalign += sz;
        total += sz;
        m_length -= sz;
        if (chain->m_misalign == chain->m_offset &&
            chain->m_offset == chain->m_size)
        {
            printf("Empty chain, recycling\n");
            it = m_chains.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return total;
}

int DataBuffer::Send(int fd)
{
    tChainList::iterator it;
    BufferChain *chain;
    size_t sz;
    ssize_t n;
    ssize_t total;
    char buf[1024];

    total = 0;
    for (it = m_chains.begin(); it != m_chains.end(); ) {
        chain = *it;
        if (chain->m_type == kSegmentType_Memory) {
            sz = chain->m_offset - chain->m_misalign;
            n = send(fd, chain->m_buffer + chain->m_misalign, sz, 0);
            if (n <= 0) {
                if (n == 0 && total > 0)
                    return total;
                return n;
            }
            total += n;
            m_length -= n;
        } else if (chain->m_type == kSegmentType_File) {
            if (chain->m_misalign > 0) {
              //  n = send(fd, chain->m_buffer)
            }
            else {
                n = read(chain->m_fd, chain->m_buffer, 4096);
                if (n <= 0) {
                    if (n == 0 && total > 0)
                        return total;
                    return n;
                }
                chain->m_misalign = n;
            }
        }
        else
            break ;
        chain->m_misalign += n;
        if (chain->m_misalign == chain->m_offset) {
            it = m_chains.erase(it);
            m_freeChains.push_back(chain);
        }
        else {
            break ;
        }

    }
    return total;
}

int DataBuffer::AddFile(int fd, size_t offset, size_t length)
{
    tChainList::iterator it;
    BufferChain *chain;

    if (offset != 0)
        return -1;

    /* Find a spare file segment chain */
    for (it = m_freeChains.begin(); it != m_freeChains.end(); ++it)
    {
        chain = *it;
        if (chain->m_type == kSegmentType_File)
            break ;
    }
    if (it == m_freeChains.end())
    {
        chain = new BufferChain;
        chain->m_type = kSegmentType_File;
        chain->m_buffer = new char[4096];
    }
    else
    {
        m_freeChains.erase(it);
    }
    chain->m_offset = offset;
    chain->m_size = length;
    chain->m_misalign = 0;
    chain->m_fd = fd;
    m_length += length;
    m_chains.push_back(chain);
    return 0;
}

void DataBuffer::AddBuffer(DataBuffer *buffer)
{
    m_length += buffer->m_length;
    m_chains.insert(m_chains.end(), buffer->m_chains.begin(), buffer->m_chains.end());
    buffer->m_length = 0;
    buffer->m_chains.clear();
}

size_t DataBuffer::GetLength() const {
    return m_length;
}

void DataBuffer::SetReadHighWatermark(uint32_t watermark) {
    m_readHighWatermark = watermark;
}

uint32_t DataBuffer::GetReadHighWatermark() const {
    return m_readHighWatermark;
}

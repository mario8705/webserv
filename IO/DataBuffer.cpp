//
// Created by Alexis Lavaud on 15/03/2023.
//

#include "DataBuffer.h"
#include <cstdlib>
#include <limits>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include "BufferChain.h"

DataBuffer::DataBuffer()
{
    m_length = 0;
    m_readHighWatermark = std::numeric_limits<size_t>::max();
    m_writeHighWatermark = std::numeric_limits<size_t>::max();
}

DataBuffer::~DataBuffer()
{
    tChainList::iterator it;

    for (it = m_chains.begin(); it != m_chains.end(); ++it)
    {
        (*it)->Release();
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
    std::memcpy(chain->m_buffer + chain->m_offset, data, n);
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

int DataBuffer::Read(int fd)
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
    n = read(fd, chain->m_buffer + chain->m_offset, avail);
    if (n > 0) {
        chain->m_offset += n;
        m_length += n;
    }
    else
    {
        perror("read");
    }
    return static_cast<int>(n);
}

BufferChain *DataBuffer::NewMemorySegment(size_t minCapacity)
{
    BufferChain *chain;

    chain = BufferChain::Allocate(minCapacity);
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
    for (it = m_chains.begin(); it != m_chains.end() && !nl; ++it)
    {
        chain = *it;

        for (i = chain->m_misalign; i < chain->m_offset && !nl; ++i, ++off)
        {
            nl = (chain->m_buffer[i] == '\n');
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
    size_t remaining;
    char *ptr;

    ptr = (char *)data;
    total = 0;
    remaining = n;
    for (it = m_chains.begin(); it != m_chains.end() && total < n; )
    {
        chain = *it;
        sz = chain->m_offset - chain->m_misalign;
        if (sz > remaining)
            sz = remaining;
        memcpy(ptr + total, chain->m_buffer + chain->m_misalign, sz);
        chain->m_misalign += sz;
        total += sz;
        remaining -= sz;
        m_length -= sz;
        if (chain->m_misalign == chain->m_offset &&
            chain->m_offset == chain->m_size)
        {
            it = m_chains.erase(it);
            chain->Release();
        }
        else
        {
            ++it;
        }
    }
    return total;
}

int DataBuffer::Write(int fd)
{
    tChainList::iterator it;
    BufferChain *chain;
    size_t sz;
    ssize_t n;
    ssize_t total;

    total = 0;
    for (it = m_chains.begin(); it != m_chains.end(); ) {
        chain = *it;
        sz = chain->m_offset - chain->m_misalign;
        n = write(fd, chain->m_buffer + chain->m_misalign, sz);
        if (n <= 0) {
            if (n == 0 && total > 0)
                return total;
            return n;
        }
        total += n;
        m_length -= n;
        chain->m_misalign += n;
        if (chain->m_misalign == chain->m_offset) {
            it = m_chains.erase(it);
            chain->Release();
        }
        else {
            break ;
        }

    }
    return total;
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

void DataBuffer::SetWriteHighWatermark(uint32_t watermark) {
    m_writeHighWatermark = watermark;
}

uint32_t DataBuffer::GetWriteHighWatermark() const {
    return m_writeHighWatermark;
}

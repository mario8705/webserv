//
// Created by Alexis Lavaud on 15/03/2023.
//

#include "BufferChain.h"

static int active_chains = 0;

BufferChain::BufferChain()
{
    printf("New chain\n");
    ++active_chains;
}

BufferChain::~BufferChain()
{
    printf("Chain deleted (active: %d)\n", --active_chains);
    delete[] m_buffer;
}

size_t BufferChain::GetFreeSpace() const {
    return m_size - m_offset;
}

void BufferChain::Release()
{
    if (m_pool.size() < kMaxPooledChains)
    {
        m_pool.push_back(this);
    }
    else
    {
        delete this;
    }
}

BufferChain *BufferChain::Allocate(size_t capacity) {
    BufferChain *chain;
    char *buffer;

    if (!m_pool.empty())
    {
        chain = m_pool.back();
        m_pool.pop_back();
    }
    else
    {
        buffer = new char[capacity];
        chain = new BufferChain;
    }
    chain->m_size = capacity;
    chain->m_buffer = buffer;
    chain->m_misalign = 0;
    chain->m_offset = 0;
    return chain;
}

const int BufferChain::kMaxPooledChains = 256;

void BufferChain::ReleasePool() {
    std::vector<BufferChain *>::const_iterator it;

    for (it = m_pool.begin(); it != m_pool.end(); ++it)
        delete *it;
    m_pool.erase(m_pool.begin(), it);
}

std::vector<BufferChain *> BufferChain::m_pool;
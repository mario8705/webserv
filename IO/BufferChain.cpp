//
// Created by Alexis Lavaud on 15/03/2023.
//

#include "BufferChain.h"

BufferChain::BufferChain()
{
}

BufferChain::~BufferChain()
{
    delete[] m_buffer;
}

size_t BufferChain::GetFreeSpace() const {
    return m_size - m_offset;
}

BufferChain *BufferChain::Allocate(size_t capacity) {
    BufferChain *chain;
    char *buffer;

    buffer = new char[capacity];
    chain = new BufferChain;
    chain->m_size = capacity;
    chain->m_buffer = buffer;
    chain->m_misalign = 0;
    chain->m_offset = 0;
    return chain;
}

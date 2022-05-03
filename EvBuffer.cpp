//
// Created by 王国栋 on 2022/5/3.
//

#include "EvBuffer.h"
#include <event2/thread.h>

EvBuffer::EvBuffer()
{
    m_EvBuffer = evbuffer_new();
    evbuffer_enable_locking(m_EvBuffer, NULL);
}

EvBuffer::EvBuffer(EvBuffer &&evBuffer)
{
    m_EvBuffer = evbuffer_new();
    evbuffer_enable_locking(m_EvBuffer, NULL);
    evbuffer_add_buffer(this->m_EvBuffer, evBuffer.m_EvBuffer);
}

EvBuffer::~EvBuffer()
{
    if (m_EvBuffer != nullptr)
    {
        evbuffer_free(m_EvBuffer);
        m_EvBuffer = nullptr;
    }
}

EvBuffer &EvBuffer::operator=(EvBuffer &&evBuffer)
{
    if (this == &evBuffer)
    {
        return *this;
    }
    evbuffer_add_buffer(this->m_EvBuffer, evBuffer.m_EvBuffer);
    return *this;
}

void EvBuffer::lock()
{
    evbuffer_lock(m_EvBuffer);
}

void EvBuffer::unlock()
{
    evbuffer_unlock(m_EvBuffer);
}

size_t EvBuffer::get_length()
{
    return evbuffer_get_length(m_EvBuffer);
}

bool EvBuffer::write(char *data, size_t len)
{
    int ret = evbuffer_add(m_EvBuffer, data, len);
    return ret == 0;
}

int EvBuffer::read(char *data, size_t len)
{
    return evbuffer_remove(m_EvBuffer, data, len);
}

void EvBuffer::extend(size_t size)
{
    evbuffer_expand(m_EvBuffer, size);
}

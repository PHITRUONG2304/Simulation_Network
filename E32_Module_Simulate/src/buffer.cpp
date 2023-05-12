#include "buffer.h"


Buffer::Buffer(const int capacity)
{
    this->pData = new uint8_t[capacity];
    this->cap = capacity;
    this->size = 0;
    this->first = 0;
    this->last = -1;
}

bool Buffer::enqueue(uint8_t _data)
{
    if (this->size < this->cap)
    {
        last = (last + 1) % this->cap;
        pData[last] = _data;
        this->size++;
        return true;
    }
    return false;
}

bool Buffer::dequeue(uint8_t &data)
{
    if (this->size > 0)
    {
        data = pData[first];
        first = (first + 1) % cap;
        this->size--;
        return true;
    }
    return false;
}

bool Buffer::lastElement(uint8_t &data)
{
    if(this->size == 0) return false;
    data = this->pData[last];
    return true;
}
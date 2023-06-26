#include "Buffer.h"

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
        this->size += 1;
        return true;
    }
    return false;
}

bool Buffer::dequeue(uint8_t &data)
{
    if (this->size > 0)
    {
        data = pData[first];
        first = (first + 1) % this->cap;
        this->size -= 1;
        return true;
    }
    return false;
}

uint8_t *Buffer::deallqueue(size_t &_size)
{
    if (this->size == 0)
    {
        _size = 0;
        return nullptr;
    }
    uint8_t *returnData = new uint8_t[this->size];
    _size = this->size;
    size_t index = 0;
    while (this->available())
    {
        uint8_t data;
        this->dequeue(data);
        *(returnData + index) = data;
        index++;
    }
    return returnData;
}
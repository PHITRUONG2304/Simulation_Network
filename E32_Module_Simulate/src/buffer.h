#ifndef _BUFFER_H_
#define _BUFFER_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Buffer
{
private:
    uint8_t *pData;
    int first, last;
    size_t cap;
    size_t size;
    volatile bool locked = false;
public:
    Buffer(const int capacity = 1024);
    ~Buffer() { delete[] pData; }
    bool enqueue(volatile uint8_t _data);
    bool dequeue(volatile uint8_t &data);
    uint8_t *deallqueue(size_t &_size);
    size_t available() { return this->size; }
};

#endif

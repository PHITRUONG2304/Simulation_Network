#ifndef _BUFFER_H_
#define _BUFFER_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <SoftwareSerial.h>

class Buffer
{
private:
    uint16_t size;
    uint16_t maxSize;
    uint8_t *buffer;

public:
    Buffer(uint16_t size = 512);
    uint16_t available() { return this->size; }
    void recvFromSerial();
    void writeToSWSerial(SoftwareSerial& swSerial);
    void recvFromSWSerial(SoftwareSerial& swSerial);
    void writeToSerial();
};

#endif

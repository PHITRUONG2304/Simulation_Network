#include "buffer.h"

Buffer::Buffer(const uint16_t size)
{
    this->maxSize = size;
    this->buffer = new uint8_t[size];
    this->size = 0;
}

void Buffer::recvFromSerial()
{
    while (Serial.available())
    {
        this->buffer[size] = Serial.read();
        this->size = (this->size + 1)%this->maxSize;
    }
}

void Buffer::writeToSWSerial(SoftwareSerial& swSerial)
{
    if(this->size == 0) return;
    swSerial.write(this->buffer, this->size);      
    memset(this->buffer, 0, this->size);
    this->size = 0;
}

void Buffer::recvFromSWSerial(SoftwareSerial& swSerial)
{
    while (swSerial.available())
    {
        this->buffer[size] = swSerial.read();
        this->size = (this->size + 1)%this->maxSize;
    }
}

void Buffer::writeToSerial()
{
    if(this->size == 0) return;
    Serial.write(this->buffer, this->size);      
    memset(this->buffer, 0, this->size);
    this->size = 0;
}
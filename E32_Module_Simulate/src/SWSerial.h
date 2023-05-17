#include "Arduino.h"
#include "Buffer.h"

enum State
{
    STARTBIT,
    ENDBIT,
    INBIT,
    IDLE
};

class SWSerial
{
private:
    volatile State w_state;
    volatile uint8_t w_count;
    volatile uint8_t w_frame;

    volatile State r_state;
    volatile uint8_t r_count;
    volatile uint8_t r_frame;

    Buffer w_buffer;
    Buffer r_buffer;

    volatile uint8_t *_receivePortRegister;
    volatile uint8_t *_transmitPortRegister;

    void setRX(int8_t rx);
    void setTX(int8_t tx);

    void initTimer();
    void initPCInterrupt();

public:
    SWSerial(uint8_t receivePin, uint8_t transmitPin);
    void begin(uint16_t baudrate = 9600);
    bool writeSerial(uint8_t _data);
    int readSerial();
    int available() { return this->r_buffer.available(); }

    void D_Write(uint8_t state);
    void setStatePCInterrupt(bool state);
    void enableInterruptToRead();
    void handleWhenWrite();
    void handleWhenRead();
    void disableInterruptToRead();
};
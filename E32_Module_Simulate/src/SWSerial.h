#include "Arduino.h"
#include "Buffer.h"
#include "SoftwareSerial.h"

#define SS_RECEIVE_BUFFER_SIZE  128
#define SS_WRITE_BUFFER_SIZE    16
#define IDLE_BITS               9 // idle 10 bits = 1 frame

enum State
{
    STARTBIT,
    ENDBIT,
    INBIT,
    IDLE
};

struct Data_block
{
    uint8_t *data;
    size_t size;
};

class SWSerial
{
private:
    static SWSerial *activeObj;

    volatile State w_state;
    volatile State r_state;

    // For write - TX
    Data_block _write_buffer[SS_WRITE_BUFFER_SIZE];
    volatile uint8_t _write_buffer_head;
    volatile uint8_t _write_buffer_tail;
    volatile size_t _write_buffer_size;

    // For receive - RX
    uint8_t _receive_buffer[SS_RECEIVE_BUFFER_SIZE];
    volatile uint8_t _receive_buffer_head;
    volatile uint8_t _receive_buffer_tail;

    void setRX(uint8_t rx);
    void setTX(uint8_t tx);

    void initTimer();
    void initPCInterrupt();

    inline void D_Write(uint8_t _state);
    inline void setStatePCInterrupt(bool _state);
    inline void setInterruptToRead(bool _state);

    inline void handleWhenWrite();
    inline void handleWhenRead();
    inline void enableInterruptToRead();

public:
    SWSerial(uint8_t receivePin, uint8_t transmitPin);
    void begin(uint16_t baudrate);
    bool write(uint8_t _data[], size_t size);
    int read();
    int available() { return ((uint16_t)(_receive_buffer_tail + SS_RECEIVE_BUFFER_SIZE - _receive_buffer_head)) % SS_RECEIVE_BUFFER_SIZE; }
    
    static inline void handleInterruptForWrite();
    static inline void handleInterruptForRead();
    static inline void startRead();
};
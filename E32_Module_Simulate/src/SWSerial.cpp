#include "SWSerial.h"
#include "BitOperations.h"


SWSerial* SWSerial::activeObj = nullptr;

SWSerial::SWSerial(uint8_t receivePin, uint8_t transmitPin)
{
    // For write - TX
    this->_write_buffer_head = 0;
    this->_write_buffer_tail = 0;
    this->_write_buffer_size = 0;
    // For receive - RX
    this->_receive_buffer_head = 0;
    this->_receive_buffer_tail = 0;
    // Setup state machine
    this->w_state = IDLE;
    this->r_state = IDLE;

    this->setRX(receivePin);
    this->setTX(transmitPin);
}

void SWSerial::setTX(uint8_t tx)
{
    digitalWrite(tx, HIGH);
    pinMode(tx, OUTPUT);
}

void SWSerial::setRX(uint8_t rx)
{
    pinMode(rx, INPUT);
    digitalWrite(rx, HIGH); // pullup for normal logic!
}

void SWSerial::begin(uint16_t baudrate)
{
    initTimer();
    initPCInterrupt();
    if(SWSerial::activeObj == nullptr)
        SWSerial::activeObj = this;
}

void SWSerial::initTimer()
{
    cli();
    // Timer at NORMAL mode
    TCCR1A = 0;
    TCCR1B = 0;
    SET_BIT0(TCCR1B); // Prescaler = 1

    // Interrupt disable
    TIMSK1 = 0;
    SET_BIT1(TIMSK1); // enable TIMER1_COMPA_vect

    // reset timer
    TCNT1 = 0;
    sei();
}
void SWSerial::initPCInterrupt()
{
    cli();
    // Enable pin change interrupt control register
    PCICR = 0;
    SET_BIT0(PCICR);
    // Enable pin change mask
    PCMSK0 = 0;
    setStatePCInterrupt(true);
    sei();
}

void SWSerial::enableInterruptToRead()
{
    this->r_state = STARTBIT;
    setInterruptToRead(true);
    OCR1B = (TCNT1 + 833) % UINT16_MAX;
}

bool SWSerial::write(uint8_t _data[], size_t _size)
{
    uint8_t next = (this->_write_buffer_tail + 1) % SS_WRITE_BUFFER_SIZE;
    if(next == this->_write_buffer_head) return false;

    this->_write_buffer[_write_buffer_tail].data = new uint8_t[_size];
    memcpy(this->_write_buffer[_write_buffer_tail].data, _data, _size);
    this->_write_buffer[_write_buffer_tail].size = _size;
    this->_write_buffer_tail = next;
    return true;
}

int SWSerial::read()
{
    if (this->_receive_buffer_head == this->_receive_buffer_tail) return -1;
    
    uint8_t dataReturn = this->_receive_buffer[_receive_buffer_head];
    this->_receive_buffer_head = (_receive_buffer_head + 1) % SS_RECEIVE_BUFFER_SIZE;
    return dataReturn;
}

void SWSerial::handleWhenWrite()
{
    static uint8_t frame = 0;
    static uint8_t count = 0;
    static uint8_t idle_count = 0;
    switch (this->w_state)
    {
    case IDLE:
        if(idle_count < IDLE_BITS) idle_count += 1;
        else 
        {
            if(this->_write_buffer_head != this->_write_buffer_tail)
            {           
                this->w_state = STARTBIT;
                frame = this->_write_buffer[_write_buffer_head].data[0];
                this->_write_buffer_size = 1;
            }
        }
        break;
    case STARTBIT:
        D_Write(LOW);
        this->w_state = INBIT;
        count = 0;
        break;
    case INBIT:
        D_Write(frame & 0x01);
        frame >>= 1;
        count += 1;
        if (count == 8)
            this->w_state = ENDBIT;
        break;
    case ENDBIT:
        D_Write(HIGH);
        if(this->_write_buffer_size < this->_write_buffer[_write_buffer_head].size)
        {
            this->w_state = STARTBIT;
            frame = this->_write_buffer[_write_buffer_head].data[this->_write_buffer_size];
            this->_write_buffer_size += 1;
        }
        else
        {
            this->w_state = IDLE;
            idle_count = 0;
            if(this->_write_buffer[_write_buffer_head].data != nullptr)
            {
                delete[] this->_write_buffer[_write_buffer_head].data;
                this->_write_buffer[_write_buffer_head].data = nullptr;
            }
            this->_write_buffer_head = (this->_write_buffer_head + 1) % SS_WRITE_BUFFER_SIZE;
        }
        break;
    default:
        break;
    }
}

void SWSerial::handleWhenRead()
{
    static uint8_t frame = 0;
    static uint8_t count = 0;
    switch (this->r_state)
    {
    case STARTBIT:
        setStatePCInterrupt(false);
        this->r_state = INBIT;
        frame = 0;
        count = 0;
        break;
    case INBIT:
        frame >>= 1;
        if (BIT2_IS_SET(PINB))
            frame |= 0x80;
        count += 1;
        if (count == 8)
            this->r_state = ENDBIT;
        break;
    case ENDBIT:
        if(((this->_receive_buffer_tail + 1) % SS_RECEIVE_BUFFER_SIZE) != this->_receive_buffer_head)
        {
            this->_receive_buffer[this->_receive_buffer_tail] = frame;
            this->_receive_buffer_tail = (this->_receive_buffer_tail + 1) % SS_RECEIVE_BUFFER_SIZE;
        }
        setStatePCInterrupt(true);
        setInterruptToRead(false);
        this->r_state = IDLE;
        break;
    default:
        break;
    }
}

inline void SWSerial::handleInterruptForWrite()
{
    if(activeObj)
        activeObj->handleWhenWrite();
}

inline void SWSerial::handleInterruptForRead()
{
    if(activeObj)
        activeObj->handleWhenRead();
}

inline void SWSerial::startRead()
{
    if(activeObj)
        activeObj->enableInterruptToRead();
}

void SWSerial::setStatePCInterrupt(bool _state)
{
    (_state == true) ? SET_BIT2(PCMSK0) : CLEAR_BIT2(PCMSK0);
}

void SWSerial::setInterruptToRead(bool _state)
{
    (_state == true) ? SET_BIT2(TIMSK1) : CLEAR_BIT2(TIMSK1);
}

void SWSerial::D_Write(uint8_t _state)
{
    (_state != 0) ? SET_BIT3(PORTB) : CLEAR_BIT3(PORTB);
}

// Interrupt handlers
ISR(TIMER1_COMPA_vect) // For write Serial
{
  // TODO
  SWSerial::handleInterruptForWrite();
  // Critical, NOT CHANGE
  OCR1A = (OCR1A + 1666) % UINT16_MAX;
}

ISR(TIMER1_COMPB_vect)
{
  // TODO
  SWSerial::handleInterruptForRead();
  // Critical, NOT CHANGE
  OCR1B = (OCR1B + 1666) % UINT16_MAX;
}

ISR(PCINT0_vect)
{
  // TODO
  SWSerial::startRead();
}
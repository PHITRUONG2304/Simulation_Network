#include "SWSerial.h"
#include "BitOperations.h"

SWSerial::SWSerial(uint8_t receivePin, uint8_t transmitPin)
{
    this->r_buffer = Buffer(128);
    this->w_buffer = Buffer(128);
    this->w_state = IDLE;
    this->r_state = IDLE;
    this->setRX(receivePin);
    this->setTX(transmitPin);
}

void SWSerial::setTX(int8_t tx)
{
    digitalWrite(tx, HIGH);
    pinMode(tx, OUTPUT);
}

void SWSerial::setRX(int8_t rx)
{
    pinMode(rx, INPUT);
    digitalWrite(rx, HIGH); // pullup for normal logic!
}

void SWSerial::begin(uint16_t baudrate)
{
    initTimer();
    initPCInterrupt();
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
    SET_BIT1(TIMSK1);

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
    SET_BIT2(PCMSK0);
    sei();
}

void SWSerial::enableInterruptToRead()
{
    setStatePCInterrupt(false);
    this->r_state = STARTBIT;
    SET_BIT2(TIMSK1); // enable TIMER1_COMPB_vect
    OCR1B = (TCNT1 + 833) % UINT16_MAX;
}
void SWSerial::disableInterruptToRead()
{
    CLEAR_BIT2(TIMSK1); // disable TIMER1_COMPB_vect
    setStatePCInterrupt(true);
}

bool SWSerial::writeSerial(uint8_t _data)
{
    return this->w_buffer.enqueue(_data);
}

int SWSerial::readSerial()
{
    if (this->r_buffer.available())
    {
        uint8_t dataReturn;
        this->r_buffer.dequeue(dataReturn);
        return dataReturn;
    }
    return -1;
}

void SWSerial::setStatePCInterrupt(bool state)
{
    state ? SET_BIT2(PCMSK0) : CLEAR_BIT2(PCMSK0);
}

void SWSerial::D_Write(uint8_t state)
{
    state ? SET_BIT3(PORTB) : CLEAR_BIT3(PORTB);
}

void SWSerial::handleWhenWrite()
{
    switch (this->w_state)
    {
    case STARTBIT:
        D_Write(LOW);
        this->w_state = INBIT;
        this->w_count = 0;
        break;
    case INBIT:
        D_Write(this->w_frame & 1);
        this->w_frame >>= 1;
        this->w_count += 1;
        if (this->w_count == 8)
            this->w_state = ENDBIT;
        break;
    case ENDBIT:
        D_Write(HIGH);
        this->w_state = this->w_buffer.dequeue(this->w_frame) ? STARTBIT : IDLE;
        break;
    case IDLE:
        this->w_state = this->w_buffer.dequeue(this->w_frame) ? STARTBIT : this->w_state;
        break;
    default:
        break;
    }
}

void SWSerial::handleWhenRead()
{
    switch (this->r_state)
    {
    case STARTBIT:
        this->r_state = INBIT;
        this->r_count = 0;
        break;
    case INBIT:
        this->r_frame >>= 1;
        if (BIT2_IS_SET(PINB))
            this->r_frame |= 0x80;
        this->r_count += 1;
        if (this->r_count == 8)
            this->r_state = ENDBIT;
        break;
    case ENDBIT:
        this->r_buffer.enqueue(this->r_frame);
        disableInterruptToRead();
        this->r_state = IDLE;
        break;
    default:
        break;
    }
}

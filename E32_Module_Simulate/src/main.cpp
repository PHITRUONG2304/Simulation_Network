#include <Arduino.h>
#include "pinsDefinition.h"
#include "configurations.h"
#include "E32_Module.h"
#include "buffer.h"
#include "SWSerial.h"

#define MAX_READ_SIZE 58
#define BYTE_TIME_115200 70 // us
#define BYTE_TIME_9600 833  // us
#define MAX_WAIT_TIMES 3

SWSerial swSerial = SWSerial(RX_SW, TX_SW); // RXD = 10 and TXD = 11

enum STATUS_E32
{
  NORMAL,
  WAKE_UP,
  POWER_SAVING,
  SLEEP
};

volatile uint32_t currentBaudrate = CONF_BAUD;
volatile STATUS_E32 currentState;

Buffer lower_Buffer = Buffer(BUFF_SIZE);
unsigned long low_buff_timestamp = 0;
uint8_t low_buff_wait_times = 0;
size_t low_buf_old_buff_size = 0;

Buffer upper_Buffer = Buffer(BUFF_SIZE);
unsigned long up_buff_timestamp = 0;
uint8_t up_buff_wait_times = 0;
size_t up_buf_old_buff_size = 0;

E32_Module e32_module;

void changeStateModule();
void handeWithFSM();
void restartSerialAtBaudrate(uint32_t _baudrate);

void setup()
{
  // put your setup code here, to run once:
  pinDirInit();
  outputInit();
  Serial.begin(STM_TO_ATMEGA_BAUD);
  swSerial.begin(ATMEGA_TO_LAP_BAUD);
  attachInterrupt(digitalPinToInterrupt(M0), changeStateModule, CHANGE);
  attachInterrupt(digitalPinToInterrupt(M1), changeStateModule, CHANGE);
  delay(5);
  currentState = SLEEP;
  digitalWrite(AUX, HIGH);
}

void loop()
{
  // put your main code here, to run repeatedly:
  handeWithFSM();
}

void handeWithFSM()
{
  switch (currentState)
  {
  case NORMAL:
    
    while(Serial.available())
      lower_Buffer.enqueue(Serial.read());
    while(swSerial.available())
      upper_Buffer.enqueue(swSerial.read());

    if (lower_Buffer.available())
    {
      if (low_buf_old_buff_size != lower_Buffer.available())
      {
        low_buff_timestamp = micros();
        low_buf_old_buff_size = lower_Buffer.available();
        low_buff_wait_times = 0;
      }
      else
      {
        if (micros() - low_buff_timestamp >= BYTE_TIME_115200)
        {
          low_buff_timestamp = micros();
          low_buff_wait_times++;
          if (low_buff_wait_times == MAX_WAIT_TIMES)
          {
            digitalWrite(AUX, LOW);
            const size_t size = lower_Buffer.available();
            uint8_t data[size];
            for (size_t i = 0; i < size; i++)
              lower_Buffer.dequeue(*(data + i));

            swSerial.write(data, size);
            digitalWrite(AUX, HIGH);
          }
        }
      }
    }

    if (upper_Buffer.available())
    {
      if (up_buf_old_buff_size != upper_Buffer.available())
      {
        up_buff_timestamp = micros();
        up_buf_old_buff_size = upper_Buffer.available();
        up_buff_wait_times = 0;
      }
      else
      {
        if (micros() - up_buff_timestamp >= BYTE_TIME_9600)
        {
          up_buff_timestamp = micros();
          up_buff_wait_times++;
          if (up_buff_wait_times == MAX_WAIT_TIMES)
          {
            digitalWrite(AUX, LOW);
            const size_t size = upper_Buffer.available();
            uint8_t data[size];
            for (size_t i = 0; i < size; i++)
              upper_Buffer.dequeue(*(data + i));
            for (size_t i = 0; i < size; i++)
              Serial.write(*(data + i));
            digitalWrite(AUX, HIGH);
          }
        }
      }
    }

    break;

  case WAKE_UP:
    // be done later
    // TODO
    break;

  case POWER_SAVING:
    // be done later
    // TODO
    break;

  case SLEEP:
    if (Serial.available())
    {
      digitalWrite(AUX, LOW);
      uint8_t params[CONF_SIZE];
      Serial.readBytes(params, CONF_SIZE);
      e32_module.setParams(params, CONF_SIZE);
      Serial.write(params, CONF_SIZE);
      digitalWrite(AUX, HIGH);
    }
    break;
  }
}

void restartSerialAtBaudrate(uint32_t _baudrate)
{
  if (currentBaudrate == _baudrate)
    return;
  Serial.end();
  Serial.begin(_baudrate);
  currentBaudrate = _baudrate;
}

void changeStateModule()
{
  uint8_t M0_status = digitalRead(M0);
  uint8_t M1_status = digitalRead(M1);

  if (M0_status == LOW && M1_status == LOW)
    currentState = NORMAL;
  else if (M0_status == HIGH && M1_status == LOW)
    currentState = WAKE_UP;
  else if (M0_status == LOW && M1_status == HIGH)
    currentState = POWER_SAVING;
  else if (M0_status == HIGH && M1_status == HIGH)
  {
    currentState = SLEEP;
    restartSerialAtBaudrate(CONF_BAUD);
    delay(2);
    return;
  }
  restartSerialAtBaudrate(STM_TO_ATMEGA_BAUD);
  delay(1);
}
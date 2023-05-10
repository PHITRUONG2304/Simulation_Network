#include <Arduino.h>
#include "pinsDefinition.h"
#include "configurations.h"
#include "E32_Module.h"
#include "buffer.h"
#include <SoftwareSerial.h>
SoftwareSerial cerr(10, 11); // RXD = 10 and TXD = 11
Buffer rx_Buffer;
Buffer tx_Buffer;

enum STATUS_E32
{
  NORMAL,
  WAKE_UP,
  POWER_SAVING,
  SLEEP
};

#define TICK_MS_9600 105
#define TICK_MS_115200 9

volatile uint32_t currentBaudrate = CONF_BAUD;
volatile STATUS_E32 currentState;

E32_Module e32_module;

void changeStateModule();
void handeWithFSM();
void restartSerialAtBaudrate(uint32_t _baudrate);

void setup()
{
  // put your setup code here, to run once:
  pinDirInit();
  outputInit();
  Serial.begin(CONF_BAUD);
  cerr.begin(ATMEGA_TO_LAP_BAUD);
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
    if (Serial.available())
    {
      // delayMicroseconds(TICK_MS_115200 * 2); // delay 2 ticks
      digitalWrite(AUX, LOW);
      unsigned long start = millis();
      rx_Buffer.recvFromSerial();
      rx_Buffer.writeToSWSerial(cerr);
      // Delay about 3ms - 4ms
      delay(3.5 - (millis() - start));
      digitalWrite(AUX, HIGH);
    }
    if(cerr.available())
    {
      digitalWrite(AUX, LOW);
      unsigned long start = millis();
      tx_Buffer.recvFromSWSerial(cerr);
      delay(5 - (millis() - start));
      tx_Buffer.writeToSerial();
      digitalWrite(AUX, HIGH);
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
      delayMicroseconds(TICK_MS_9600 * 2); // delay 2 ticks
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

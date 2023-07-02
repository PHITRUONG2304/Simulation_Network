#ifndef _PINSDEFINITION_H_
#define _PINSDEFINITION_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

/*-------------INPUT------------*/
#define M0                      4   // PD4
#define M1                      2   // PD2

/*-------------OUTPUT-----------*/
#define UPLINK_LED              8   // PB0
#define ERR_LED                 9   // PB1
#define AUX                     3   // PD3

/*-----------RX-TX-SW----------*/
#define RX_SW                   10  // PB2
#define TX_SW                   11  // PB3

void pinDirInit();
void outputInit();

#endif
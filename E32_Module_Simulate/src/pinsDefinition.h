#ifndef _PINSDEFINITION_H_
#define _PINSDEFINITION_H_

#include <Arduino.h>

/*-------------INPUT------------*/
#define M0                      4
#define M1                      2

/*-------------OUTPUT-----------*/
#define UPLINK_LED              8
#define ERR_LED                 9
#define AUX                     3

void pinDirInit();
void outputInit();

#endif
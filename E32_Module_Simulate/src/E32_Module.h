#ifndef _E32_MODULE_H_
#define _E32_MODULE_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#define CONF_SIZE 6

enum PARAMS_ORDER
{
    HEAD,
    ADDH,
    ADDL,
    SPED,
    CHAN,
    OPTION
};

class E32_Module
{
private:
    uint8_t paramsList[CONF_SIZE];
public:
    E32_Module();
    void setParams(uint8_t *params, uint8_t size);
    void getParams(uint8_t *buffer);
};

#endif
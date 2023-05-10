#include "E32_Module.h"

E32_Module::E32_Module()
{
    this->paramsList[HEAD] = 0xC0;
    this->paramsList[ADDH] = 0x00;
    this->paramsList[ADDL] = 0x00;
    this->paramsList[SPED] = 0x1A;
    this->paramsList[CHAN] = 0x17;
    this->paramsList[OPTION] = 0x44;
}
void E32_Module::setParams(uint8_t *params, uint8_t size)
{
    if (size != CONF_SIZE)
        return;
    for (int i = 0; i < size; i++)
        this->paramsList[i] = params[i];
}
void E32_Module::getParams(uint8_t *buffer)
{
    for(int i = 0; i < CONF_SIZE; i++)
        buffer[i] = this->paramsList[i];
}
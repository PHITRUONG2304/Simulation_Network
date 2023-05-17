#include "pinsDefinition.h"

void pinDirInit()
{
    pinMode(M0, INPUT_PULLUP);
    pinMode(M1, INPUT_PULLUP);
    pinMode(UPLINK_LED, OUTPUT);
    pinMode(ERR_LED, OUTPUT);
    pinMode(AUX, OUTPUT);
}

void outputInit()
{
    digitalWrite(M0, HIGH);
    digitalWrite(M1, HIGH);
    digitalWrite(UPLINK_LED, LOW);
    digitalWrite(ERR_LED, LOW);
    digitalWrite(AUX, LOW);
}
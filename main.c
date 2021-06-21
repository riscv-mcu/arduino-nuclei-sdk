#include "Arduino.h"

int led_pin = 13;

void setup()
{
    pinMode(led_pin, OUTPUT);
}

void loop()
{
    digitalWrite(led_pin, HIGH);
    delay(1000);

    digitalWrite(led_pin, LOW);
    delay(1000);
}
#include <Wire.h>

const int ADDR = 0x27;
const char *MESSAGE = "Hello World!";

void setup()
{
    Wire.begin();
    Wire.setClock(400000); // 400 kbit - fast mode
}

void loop()
{
    Wire.beginTransmission(ADDR);
    Wire.print(MESSAGE);
    Wire.endTransmission();

    delay(100);
}
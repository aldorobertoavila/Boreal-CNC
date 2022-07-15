#include <A4988.h>
#include <SPI.h>

#define CS_RS1_PIN 27
#define CS_RS2_PIN 26
#define CS_RS3_PIN 25

SPIClass spi(VSPI);

ShiftRegisterMotorInterface SHIFT_REGISTER_X(spi, CS_RS1_PIN);
ShiftRegisterMotorInterface SHIFT_REGISTER_Y(spi, CS_RS2_PIN);
ShiftRegisterMotorInterface SHIFT_REGISTER_Z(spi, CS_RS3_PIN);

A4988 DRIVER_X(SHIFT_REGISTER_X);
A4988 DRIVER_Y(SHIFT_REGISTER_Y);
A4988 DRIVER_Z(SHIFT_REGISTER_Z);

const unsigned int TARGET = 1000;
 
void setup() 
{
    Serial.begin(115200);

    DRIVER_X.setMaxSpeed(300);
    DRIVER_Y.setMaxSpeed(300);
    DRIVER_Z.setMaxSpeed(300);

    DRIVER_X.setAcceleration(100);
    DRIVER_Y.setAcceleration(100);
    DRIVER_Z.setAcceleration(100);

    spi.begin();

    pinMode(CS_RS1_PIN, OUTPUT);
    pinMode(CS_RS2_PIN, OUTPUT);
    pinMode(CS_RS3_PIN, OUTPUT);
}

void loop() 
{ 
    DRIVER_X.run();
    DRIVER_Y.run();
    DRIVER_Z.run();

    if(DRIVER_X.getCurrentPosition() == TARGET)
    {
      DRIVER_X.setSpeed(-200);
      DRIVER_X.moveTo(0);

      delay(1000);
    }

    if(DRIVER_X.getCurrentPosition() == 0)
    {
      DRIVER_X.setSpeed(200);
      DRIVER_X.moveTo(TARGET);

      delay(1000);
    }
}
#include <StepperMotor.h>
#include <SPI.h>

#define CS_RS1_PIN 27
#define CS_RS2_PIN 26
#define CS_RS3_PIN 25
#define CS_RS4_PIN 33

SPIClass spi(VSPI);

ShiftRegisterMotorInterface SHIFT_REGISTER_X1(spi, CS_RS1_PIN, LSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_X2(spi, CS_RS2_PIN, LSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Y(spi, CS_RS3_PIN, LSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Z(spi, CS_RS4_PIN, LSBFIRST);

BilateralMotorInterface BILATERAL_X(SHIFT_REGISTER_X1, SHIFT_REGISTER_X2);

StepperMotor DRIVER_X(BILATERAL_X);
StepperMotor DRIVER_Y(SHIFT_REGISTER_Y);
StepperMotor DRIVER_Z(SHIFT_REGISTER_Z);

const float INITIAL_SPEED = 200;
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
  pinMode(CS_RS4_PIN, OUTPUT);
}

void loop()
{
  DRIVER_X.run();
  DRIVER_Y.run();
  DRIVER_Z.run();

  if (DRIVER_X.getCurrentPosition() == TARGET && DRIVER_Y.getCurrentPosition() == TARGET && DRIVER_Y.getCurrentPosition() == TARGET)
  {
    DRIVER_X.setSpeed(-INITIAL_SPEED);
    DRIVER_X.moveTo(0);

    DRIVER_Y.setSpeed(-INITIAL_SPEED);
    DRIVER_Y.moveTo(0);

    DRIVER_Z.setSpeed(-INITIAL_SPEED);
    DRIVER_Z.moveTo(0);

    delay(1000);
  }

  if (DRIVER_X.getCurrentPosition() == 0)
  {
    DRIVER_X.setSpeed(INITIAL_SPEED);
    DRIVER_X.moveTo(TARGET);

    DRIVER_Y.setSpeed(INITIAL_SPEED);
    DRIVER_Y.moveTo(TARGET);

    DRIVER_Z.setSpeed(INITIAL_SPEED);
    DRIVER_Z.moveTo(TARGET);

    delay(1000);
  }
}
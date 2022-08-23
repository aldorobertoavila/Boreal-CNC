#include <Cartesian.h>
#include <Command.h>
#include <LimitSwitch.h>
#include <StepperMotor.h>
#include <SPI.h>

#define LSR_PIN 13

#define SW_X_PIN 36
#define SW_Y_PIN 39
#define SW_Z_PIN 34

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

LimitSwitch SW_X(SW_X_PIN);
LimitSwitch SW_Y(SW_Y_PIN);
LimitSwitch SW_Z(SW_Z_PIN);

Cartesian CARTESIAN;
Laser LASER(LSR_PIN);

AutohomeCommand AUTOHOME(CARTESIAN, LASER);

void setup()
{
    Serial.begin(115200);

    spi.begin();

    pinMode(CS_RS1_PIN, OUTPUT);
    pinMode(CS_RS2_PIN, OUTPUT);
    pinMode(CS_RS3_PIN, OUTPUT);
    pinMode(CS_RS4_PIN, OUTPUT);

    pinMode(SW_X_PIN, INPUT);
    pinMode(SW_Y_PIN, INPUT);
    pinMode(SW_Z_PIN, INPUT);

    DRIVER_X.setMaxSpeed(600);
    DRIVER_Y.setMaxSpeed(600);
    DRIVER_Z.setMaxSpeed(600);

    DRIVER_X.setAcceleration(300);
    DRIVER_Y.setAcceleration(300);
    DRIVER_Z.setAcceleration(300);

    SW_X.setDebounceTime(10);
    SW_Y.setDebounceTime(10);
    SW_Z.setDebounceTime(10);

    CARTESIAN.setStepperMotor(Axis::X, std::make_shared<StepperMotor>(&DRIVER_X));
    CARTESIAN.setStepperMotor(Axis::Y, std::make_shared<StepperMotor>(&DRIVER_Y));
    CARTESIAN.setStepperMotor(Axis::Z, std::make_shared<StepperMotor>(&DRIVER_Z));

    CARTESIAN.setLimitSwitch(Axis::X, std::make_shared<LimitSwitch>(&SW_X));
    CARTESIAN.setLimitSwitch(Axis::Y, std::make_shared<LimitSwitch>(&SW_Y));
    CARTESIAN.setLimitSwitch(Axis::Z, std::make_shared<LimitSwitch>(&SW_Z));

    CARTESIAN.setMinStepsPerMillimeter(Axis::X, 5);
    CARTESIAN.setMinStepsPerMillimeter(Axis::Y, 5);
    CARTESIAN.setMinStepsPerMillimeter(Axis::Z, 25);

    CARTESIAN.setStepsPerMillimeter(Axis::X, 5);
    CARTESIAN.setStepsPerMillimeter(Axis::Y, 5);
    CARTESIAN.setStepsPerMillimeter(Axis::Z, 25);

    CARTESIAN.setDimension(Axis::X, 400);
    CARTESIAN.setDimension(Axis::Y, 420);
    CARTESIAN.setDimension(Axis::Z, 95);

    AUTOHOME.start();
}

void loop()
{
    Status status = AUTOHOME.status();

    if (status == Status::CONTINUE)
    {
        AUTOHOME.execute();
    }
}

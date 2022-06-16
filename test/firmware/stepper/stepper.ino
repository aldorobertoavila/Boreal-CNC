#include <AccelStepper.h>

#define DIR_PIN 2
#define STEP_PIN 3

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

void setup()
{
    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);

    stepper.setAcceleration(100);
    stepper.setMaxSpeed(1000);
}

void loop()
{
    if (stepper.distanceToGo() == 0)
      stepper.moveTo(-stepper.currentPosition());

    stepper.run();
}

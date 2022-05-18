#include <Arduino.h>
#include <AccelStepper.h>

#define STEP_X1_PIN 2
#define STEP_X2_PIN 3
#define STEP_Y_PIN 4
#define STEP_Z_PIN 5

#define DIR_X1_PIN 6
#define DIR_X2_PIN 7
#define DIR_Y_PIN 8
#define DIR_Z_PIN 9

AccelStepper stepperX1(AccelStepper::DRIVER, STEP_X1_PIN, DIR_X1_PIN);
AccelStepper stepperX2(AccelStepper::DRIVER, STEP_X2_PIN, DIR_X2_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, STEP_Y_PIN, DIR_Y_PIN);
AccelStepper stepperZ(AccelStepper::DRIVER, STEP_Z_PIN, DIR_Z_PIN);

const int MAX_SPEED = 1000;
const int MAX_ACCEL = 500;

void setup() {
  stepperX1.setMaxSpeed(MAX_SPEED);
  stepperX1.setAcceleration(MAX_ACCEL);
  stepperX1.setCurrentPosition(0);

  stepperX2.setMaxSpeed(MAX_SPEED);
  stepperX2.setAcceleration(MAX_ACCEL);
  stepperX2.setCurrentPosition(0);

  stepperY.setMaxSpeed(MAX_SPEED);
  stepperY.setAcceleration(MAX_ACCEL);
  stepperY.setCurrentPosition(0);

  stepperZ.setMaxSpeed(MAX_SPEED);
  stepperZ.setAcceleration(MAX_ACCEL);
  stepperZ.setCurrentPosition(0);
}

void loop() {
  /*
  Non-blocking example
  
  stepperX1.moveTo(800);
  stepperX2.moveTo(800);
  stepperY.moveTo(800);
  stepperZ.moveTo(800);
  
  while (stepperX1.currentPosition() != 0 || stepperX2.currentPosition() != 0 || stepperY.currentPosition() != 0 || stepperZ.currentPosition() != 0) {
    stepperX1.run();
    stepperX2.run();
    stepperY.run();
    stepperZ.run();
  }
  */
}
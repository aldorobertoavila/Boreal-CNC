#include <AccelStepper.h>
#include <Arduino.h>
#include <Axis.h>
#include <Cartesian.h>
#include <CNC.h>

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

AccelStepper motors_x[] = {stepperX1, stepperX2};
AccelStepper motors_y[] = {stepperY};
AccelStepper motors_z[] = {stepperZ};

Axis X(motors_x, 2);
Axis Y(motors_y);
Axis Z(motors_z);

Axis axes[] = {X, Y, Z};

Cartesian cartesian(axes, 3, Cartesian::ABSOLUTE);
CNC cnc(cartesian);

/*GCode gcode();
GUI lcd();*/

void setup() {
  /*gcode.begin();a
  cnc.begin();
  lcd.begin();

  lcd.on(GUI.LIST_SD, &gcode.list);
  lcd.on(GUI.RELEASE_SD, &gcode.release);
  lcd.on(GUI.SELECT_FILE, &gcode.select);
  lcd.on(GUI.FAN_ON, &cnc.fan_on);
  lcd.on(GUI.FAN_OFF, &cnc.fan_off);

  lcd.on(GUI.PAUSE, &cnc.pause);
  lcd.on(GUI.RESUME, &cnc.resume);
  lcd.on(GUI.START, &cnc.start);
  lcd.on(GUI.STOP, &cnc.stop);*/
}

void loop() {
  /*lcd.update();*/
  cnc.update();
}
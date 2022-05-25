#include <AccelStepper.h>
#include <Arduino.h>
#include <Axis.h>
#include <Cartesian.h>
#include <CNC.h>
#include <Explorer.h>
#include <GCode.h>

#define STEP_X1_PIN 0
#define STEP_X2_PIN 0
#define STEP_Y_PIN 0
#define STEP_Z_PIN 0

#define DIR_X1_PIN 0
#define DIR_X2_PIN 0
#define DIR_Y_PIN 0
#define DIR_Z_PIN 0

#define CS_PIN D8

const char CONFIG_FILE[13] = "settings.cfg";

AccelStepper stepperX1(AccelStepper::DRIVER, STEP_X1_PIN, DIR_X1_PIN);
AccelStepper stepperX2(AccelStepper::DRIVER, STEP_X2_PIN, DIR_X2_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, STEP_Y_PIN, DIR_Y_PIN);
AccelStepper stepperZ(AccelStepper::DRIVER, STEP_Z_PIN, DIR_Z_PIN);

AccelStepper motors_x[2] = {stepperX1, stepperX2};
AccelStepper motors_y[1] = {stepperY};
AccelStepper motors_z[1] = {stepperZ};

Axis X(motors_x, 2);
Axis Y(motors_y);
Axis Z(motors_z);

Axis axes[3] = {X, Y, Z};

Cartesian cartesian(axes, 3, Cartesian::ABSOLUTE);
CNC cnc(cartesian);

Explorer explorer(CONFIG_FILE, 127);
GCode gcode(explorer);

void start(const char *path)
{
  // Bytestream stream = explorer.openStream(path);
  // gcode.start(stream);
}

void select()
{
  // Dir *directories = explorer.listDir("/", 0);
  // TODO: map directories to items
  // MenuItem *items
  // for items
  //    selectItem.onClicked(&select);
  // lcd.menu(SELECT, items)
}

void setup()
{
  // lcd.begin();
  // MenuItem *items = { selectItem };
  // lcd.menu(OPTIONS, items);
  // selectItem.onClicked(&select);
}

void loop()
{
  // lcd.update();
  // gcode.update();
  // while(gcode.available())
  //    command = gcode.get();
  //    Serial.println(command.getBuffer())
  //    cnc.push(command);
  // cnc.update();
}
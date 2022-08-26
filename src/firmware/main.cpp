#include <Cartesian.h>
#include <Command.h>
#include <Process.h>
#include <LimitSwitch.h>
#include <LiquidScreen.h>
#include <Rotary.h>
#include <StepperMotor.h>

#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include <memory>
#include <queue>

using namespace std;

#define CMD_QUEUE_SIZE 10
#define MAX_FILES_SIZE 20

#define EN_CLICK_DEBOUNCE 800
#define EN_ROT_DEBOUNCE 25

#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

#define FEED_RATE_MOVE_AXIS 1200

#define LCD_ICON_CHAR 0
#define LCD_GAUGE_FILL_1 1
#define LCD_GAUGE_FILL_2 2
#define LCD_GAUGE_FILL_3 3
#define LCD_GAUGE_FILL_4 4
#define LCD_GAUGE_LEFT 5
#define LCD_GAUGE_RIGHT 6
#define LCD_GAUGE_EMPTY 7

#define LCD_MENU_COL 2
#define LCD_ZERO_COL 0
#define LCD_ZERO_ROW 0

#define ACCEL_LBL_COL 3
#define ACCEL_VAL_COL 8

#define MOVE_LBL_COL 3
#define MOVE_VAL_COL 7

#define LSR_LBL_COL 3
#define LSR_VAL_COL 8

#define POS_LBL_X_COL 0
#define POS_LBL_Y_COL 6
#define POS_LBL_Z_COL 12

#define POS_VAL_X_COL 1
#define POS_VAL_Y_COL 7
#define POS_VAL_Z_COL 13

#define PROC_ICON_VAL_COL 0
#define PROC_NAME_LBL_COL 2
#define PROC_TIME_LBL_COL 15

#define PROG_BAR_COL 0
#define PROG_VAL_COL 15

#define STEPS_LBL_COL 3
#define STEPS_VAL_COL 8

#define VELOCITY_LBL_COL 4
#define VELOCITY_VAL_COL 8

#define MOVE_AXES_VAL_SIZE 4
#define PROG_VAL_SIZE 4
#define POS_VAL_SIZE 4

#define EN_CLK_PIN 4
#define EN_DT_PIN 2
#define EN_SW_PIN 17

#define LSR_PIN 13

#define SW_X_PIN 36
#define SW_Y_PIN 39
#define SW_Z_PIN 34

#define CS_RS1_PIN 27
#define CS_RS2_PIN 26
#define CS_RS3_PIN 25
#define CS_RS4_PIN 33

using namespace std;

enum ScreenID
{
  ABOUT,
  ACCEL,
  CARD,
  CTRL,
  HOME_OFFSETS,
  INFO,
  MAIN,
  MOTION,
  MOVE_AXES,
  MOVE_AXIS,
  MOVE_X,
  MOVE_Y,
  MOVE_Z,
  PREP,
  SET_ACCEL,
  SET_POWER,
  SET_STEPS,
  SET_VEL,
  STEPS,
  VEL
};

enum MainOption
{
  INFO_MENU_RTN,
  PREP_MENU_RTN,
  CTRL_MENU_RTN,
  NO_TF_CARD_OP,
  START_FROM_TF_OP,
  PAUSE_PROC_OP,
  RESUME_PROC_OP,
  STOP_PROC_OP,
  ABOUT_CNC_OP
};

byte ARROW_CHAR[8] = {B00000, B00100, B00110, B11111, B11111, B00110, B00100, B00000};
byte PLAY_CHAR[8] = {B00000, B10000, B11000, B11100, B11110, B11100, B11000, B10000};
byte PAUSE_CHAR[8] = {B00000, B11011, B11011, B11011, B11011, B11011, B11011, B00000};
byte STOP_CHAR[8] = {B00000, B00000, B11110, B11110, B11110, B11110, B00000, B00000};

byte GAUGE_EMPTY[8] = {B11111, B00000, B00000, B00000, B00000, B00000, B00000, B11111};
byte GAUGE_FILL_1[8] = {B11111, B10000, B10000, B10000, B10000, B10000, B10000, B11111};
byte GAUGE_FILL_2[8] = {B11111, B11000, B11000, B11000, B11000, B11000, B11000, B11111};
byte GAUGE_FILL_3[8] = {B11111, B11100, B11100, B11100, B11100, B11100, B11100, B11111};
byte GAUGE_FILL_4[8] = {B11111, B11110, B11110, B11110, B11110, B11110, B11110, B11111};
byte GAUGE_FILL_5[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};

byte GAUGE_LEFT[8] = {B01111, B10000, B10000, B10000, B10000, B10000, B10000, B01111};
byte GAUGE_RIGHT[8] = {B11110, B00001, B00001, B00001, B00001, B00001, B00001, B11110};

byte GAUGE_MASK_LEFT[8] = {B01111, B11111, B11111, B11111, B11111, B11111, B11111, B01111};
byte GAUGE_MASK_RIGHT[8] = {B11110, B11111, B11111, B11111, B11111, B11111, B11111, B11110};

const float DEFAULT_MIN_ACCEL_X = 40; // Stepper X minimum mm/s^2
const float DEFAULT_MIN_ACCEL_Y = 40; // Stepper Y minimum mm/s^2
const float DEFAULT_MIN_ACCEL_Z = 40; // Stepper Z minimum mm/s^2

const float DEFAULT_MAX_ACCEL_X = 100; // Stepper X maximum mm/s^2
const float DEFAULT_MAX_ACCEL_Y = 100; // Stepper Y maximum mm/s^2
const float DEFAULT_MAX_ACCEL_Z = 100; // Stepper Z maximum mm/s^2

const float DEFAULT_MAX_SPEED_X = 60; // Stepper X mm/s
const float DEFAULT_MAX_SPEED_Y = 60; // Stepper Y mm/s
const float DEFAULT_MAX_SPEED_Z = 60; // Stepper Z mm/s

const long DEFAULT_MIN_STEPS_X = 5;  // Stepper X minimum steps/mm (full res)
const long DEFAULT_MIN_STEPS_Y = 5;  // Stepper Y minimum steps/mm (full res)
const long DEFAULT_MIN_STEPS_Z = 25; // Stepper Z minimum steps/mm (full res)

const float DEFAULT_MAX_STEPS_X = 80;  // Stepper X maximum steps/mm (sixteenth res)
const float DEFAULT_MAX_STEPS_Y = 80;  // Stepper Y maximum steps/mm (sixteenth res)
const float DEFAULT_MAX_STEPS_Z = 400; // Stepper Z maximum steps/mm (sixteenth res)

const long DEFAULT_STEPS_X = 10; // Stepper X steps/mm (half res)
const long DEFAULT_STEPS_Y = 10; // Stepper Y steps/mm (half res)
const long DEFAULT_STEPS_Z = 50; // Stepper Z steps/mm (half res)

const float DIMENSIONS_X = 400; // X axis dimensions in mm
const float DIMENSIONS_Y = 420; // Y axis dimensions in mm
const float DIMENSIONS_Z = 95;  // Z axis dimensions in mm

const uint8_t LSR_MIN_POWER = 0;   // Minimum laser power (PWM)
const uint8_t LSR_MAX_POWER = 150; // Maximum laser power (PWM)

const unsigned long DEBOUNCE_TIME = 10;
const int PROCESS_EXPIRED = 3000;
const int CHAR_PIXEL_WIDTH = 5;
const int EMPTY_SPACE = 255;
const int GAUGE_SIZE = 14;
const int ICON_SIZE = 1;

String EMPTY_ROW("                    ");
String SYSTEM_VOLUME("System Volume Information");

char INTERNAL_FILE_EXT = '~';
String GCODE_FILE_EXT(".gcode");

Cartesian cartesian;
RTC rtc;

Laser laser(LSR_PIN);
LCD lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);
SPIClass spi(VSPI);

ShiftRegisterMotorInterface SHIFT_REGISTER_X1(spi, CS_RS1_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_X2(spi, CS_RS2_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Y(spi, CS_RS3_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Z(spi, CS_RS4_PIN, MSBFIRST);

BilateralMotorInterface BILATERAL_X(SHIFT_REGISTER_X1, SHIFT_REGISTER_X2);

StepperMotorPtr STEPPER_X = std::make_shared<StepperMotor>(BILATERAL_X);
StepperMotorPtr STEPPER_Y = std::make_shared<StepperMotor>(SHIFT_REGISTER_Y);
StepperMotorPtr STEPPER_Z = std::make_shared<StepperMotor>(SHIFT_REGISTER_Z);

LimitSwitchPtr SWITCH_X = std::make_shared<LimitSwitch>(SW_X_PIN);
LimitSwitchPtr SWITCH_Y = std::make_shared<LimitSwitch>(SW_Y_PIN);
LimitSwitchPtr SWITCH_Z = std::make_shared<LimitSwitch>(SW_Z_PIN);

LiquidScreen INFO_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen ABOUT_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);

LiquidScreen MOVE_AXIS_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_ACCEL_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_POWER_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_STEPS_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_VEL_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MAIN_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu PREP_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CTRL_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CARD_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOVE_AXES_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu HOME_OFFSETS_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOVE_X_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Y_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Z_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu ACCEL_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu VEL_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOTION_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu STEPS_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidLinePtr moveAxisText;
LiquidLinePtr moveAxisValue;

LiquidLinePtr processIconValue;
LiquidLinePtr processNameValue;
LiquidLinePtr processTimeValue;

LiquidLinePtr progressBar;
LiquidLinePtr progressValue;

LiquidLinePtr setAccelerationText;
LiquidLinePtr setAccelerationValue;

LiquidLinePtr setPowerText;
LiquidLinePtr setPowerValue;

LiquidLinePtr setStepsText;
LiquidLinePtr setStepsValue;

LiquidLinePtr setVelocityText;
LiquidLinePtr setVelocityValue;

LiquidLinePtr positionValueX;
LiquidLinePtr positionValueY;
LiquidLinePtr positionValueZ;

byte gauge_left_dynamic[8];
byte gauge_right_dynamic[8];

String filePaths[MAX_FILES_SIZE];
File root;

ScreenID currentScreen;
ScreenID prevScreen;

queue<std::shared_ptr<Command>> commands;
queue<std::shared_ptr<Process>> processes;

std::shared_ptr<Command> cmd;
std::shared_ptr<Process> proc;

LiquidScreen *getScreen(ScreenID screenIndex)
{
  switch (screenIndex)
  {
  case ScreenID::ABOUT:
    return &ABOUT_SCREEN;
  case ScreenID::ACCEL:
    return &ACCEL_MENU_SCREEN;
  case ScreenID::CARD:
    return &CARD_MENU_SCREEN;
  case ScreenID::CTRL:
    return &CTRL_MENU_SCREEN;
  case ScreenID::HOME_OFFSETS:
    return &HOME_OFFSETS_MENU_SCREEN;
  case ScreenID::INFO:
    return &INFO_SCREEN;
  case ScreenID::MAIN:
    return &MAIN_MENU_SCREEN;
  case ScreenID::MOTION:
    return &MOTION_MENU_SCREEN;
  case ScreenID::MOVE_AXES:
    return &MOVE_AXES_MENU_SCREEN;
  case ScreenID::MOVE_AXIS:
    return &MOVE_AXIS_SCREEN;
  case ScreenID::MOVE_X:
    return &MOVE_X_MENU_SCREEN;
  case ScreenID::MOVE_Y:
    return &MOVE_Y_MENU_SCREEN;
  case ScreenID::MOVE_Z:
    return &MOVE_Z_MENU_SCREEN;
  case ScreenID::PREP:
    return &PREP_MENU_SCREEN;
  case ScreenID::SET_ACCEL:
    return &SET_ACCEL_SCREEN;
  case ScreenID::SET_POWER:
    return &SET_POWER_SCREEN;
  case ScreenID::SET_STEPS:
    return &SET_STEPS_SCREEN;
  case ScreenID::SET_VEL:
    return &SET_VEL_SCREEN;
  case ScreenID::STEPS:
    return &STEPS_MENU_SCREEN;
  case ScreenID::VEL:
    return &VEL_MENU_SCREEN;
  default:
    return nullptr;
  }
}

std::string pad_right(std::string str, size_t s)
{
  if (str.size() < s)
  {
    return str + std::string(s - str.size(), ' ');
  }

  return str;
}

String formatPosition(Axis axis)
{
  float u = cartesian.getTargetPosition(axis);

  Unit unit = cartesian.getUnit();
  char buf[POS_VAL_SIZE];

  switch (unit)
  {
  case Unit::INCH:
    snprintf(buf, POS_VAL_SIZE, "%.1f", u);
    break;

  case Unit::MILLIMETER:
    snprintf(buf, POS_VAL_SIZE, "%i", (int)u);
    break;
  }

  return pad_right(buf, POS_VAL_SIZE).c_str();
}

String formatProgress(uint8_t progress)
{
  char buf[PROG_VAL_SIZE];
  snprintf(buf, PROG_VAL_SIZE, "%i%%", progress);

  return pad_right(buf, PROG_VAL_SIZE).c_str();
}

void updateProgressBar(uint8_t progress)
{
  char gaugeBuffer[GAUGE_SIZE];

  float unitsPerPixel = (GAUGE_SIZE * CHAR_PIXEL_WIDTH) / 100.0;
  int valueInPixels = round(progress * unitsPerPixel);
  int moveOffset = (CHAR_PIXEL_WIDTH - 1) - ((valueInPixels - 1) % CHAR_PIXEL_WIDTH);

  for (int i = 0; i < 8; i++)
  {
    if (valueInPixels < CHAR_PIXEL_WIDTH)
    {
      gauge_left_dynamic[i] = (GAUGE_FILL_5[i] << moveOffset) | GAUGE_LEFT[i];
    }
    else
    {
      gauge_left_dynamic[i] = GAUGE_FILL_5[i];
    }

    gauge_left_dynamic[i] = gauge_left_dynamic[i] & GAUGE_MASK_LEFT[i];
  }

  for (int i = 0; i < 8; i++)
  {
    if (valueInPixels > GAUGE_SIZE * CHAR_PIXEL_WIDTH - CHAR_PIXEL_WIDTH)
    {
      gauge_right_dynamic[i] = (GAUGE_FILL_5[i] << moveOffset) | GAUGE_RIGHT[i];
    }
    else
    {
      gauge_right_dynamic[i] = GAUGE_RIGHT[i];
    }

    gauge_right_dynamic[i] = gauge_right_dynamic[i] & GAUGE_MASK_RIGHT[i];
  }

  lcd.createChar(LCD_GAUGE_LEFT, gauge_left_dynamic);
  lcd.createChar(LCD_GAUGE_RIGHT, gauge_right_dynamic);

  for (int i = 0; i < GAUGE_SIZE; i++)
  {
    if (i == 0)
    {
      gaugeBuffer[i] = LCD_GAUGE_LEFT;
    }
    else if (i == GAUGE_SIZE - 1)
    {
      gaugeBuffer[i] = LCD_GAUGE_RIGHT;
    }
    else
    {
      if (valueInPixels <= i * CHAR_PIXEL_WIDTH)
      {
        gaugeBuffer[i] = LCD_GAUGE_EMPTY;
      }
      else if (valueInPixels > i * CHAR_PIXEL_WIDTH && valueInPixels < (i + 1) * CHAR_PIXEL_WIDTH)
      {
        gaugeBuffer[i] = CHAR_PIXEL_WIDTH - moveOffset;
      }
      else
      {
        gaugeBuffer[i] = EMPTY_SPACE;
      }
    }
  }

  progressBar->setText(gaugeBuffer);
  progressBar->display(lcd);
}

void updateInfoScreen()
{
  if (currentScreen != ScreenID::INFO)
  {
    return;
  }

  String xText = formatPosition(Axis::X);

  if (positionValueX->getText() != xText)
  {
    positionValueX->setText(xText);
    positionValueX->display(lcd);
    return;
  }
  String yText = formatPosition(Axis::Y);

  if (positionValueY->getText() != yText)
  {
    positionValueY->setText(yText);
    positionValueY->display(lcd);
    return;
  }

  String zText = formatPosition(Axis::Z);

  if (positionValueZ->getText() != zText)
  {
    positionValueZ->setText(zText);
    positionValueZ->display(lcd);
    return;
  }

  String time = proc->getTime();

  if (proc->getPreviousTime() != time)
  {
    processTimeValue->setText(time);
    processTimeValue->display(lcd);
    return;
  }

  uint8_t prevProgress = proc->getPreviousProgress();
  uint8_t progress = proc->getProgress();

  if (progress != prevProgress)
  {
    String formatted = formatProgress(progress);

    if (progressValue->getText() != formatted)
    {
      progressValue->setText(formatted);
      progressValue->display(lcd);

      updateProgressBar(progress);
      return;
    }
  }
}

void display(ScreenID screenIndex, bool forcePosition)
{
  LiquidScreen *screen = getScreen(screenIndex);

  if (screen)
  {
    if (forcePosition)
    {
      rotary.setPosition(screen->getCurrentLineIndex());
    }
    else
    {
      screen->setCurrentLine(0);
      rotary.setPosition(0);
    }

    prevScreen = currentScreen;
    currentScreen = screenIndex;
    screen->display();
  }
}

void displayMenu(ScreenID screenIndex, bool forcePosition)
{
  lcd.createChar(LCD_ICON_CHAR, ARROW_CHAR);

  display(screenIndex, forcePosition);
}

void displayInfoScreen()
{
  if (proc)
  {
    processNameValue->setText(proc->getName());
    processTimeValue->setText(proc->getTime());

    Status status = proc->status();

    if (status != Status::STOPPED)
    {
      positionValueX->setText(formatPosition(Axis::X));
      positionValueY->setText(formatPosition(Axis::Y));
      positionValueZ->setText(formatPosition(Axis::Z));

      uint8_t progress = proc->getProgress();

      updateProgressBar(progress);
      progressValue->setText(formatProgress(progress));

      display(ScreenID::INFO, false);
      return;
    }
  }
  else
  {
    processIconValue->setText("");
    processNameValue->setText("Ready!");
    processTimeValue->setText("00:00");
  }

  positionValueX->setText("0");
  positionValueY->setText("0");
  positionValueZ->setText("0");

  updateProgressBar(0);
  progressValue->setText("0%");

  display(ScreenID::INFO, false);
}

void displayAccelerationScreen()
{
  Axis axis = cartesian.getCurrentAxis();
  int accel = cartesian.getAcceleration(axis);
  String newText = String(accel);

  switch (axis)
  {
  case Axis::X:
    setAccelerationText->setText("Set X mm/s^2");
    setAccelerationValue->setText(newText);
    rotary.setBoundaries(DEFAULT_MIN_ACCEL_X, DEFAULT_MAX_ACCEL_X);
    rotary.setPosition(accel);
    break;

  case Axis::Y:
    setAccelerationText->setText("Set Y mm/s^2");
    setAccelerationValue->setText(newText);
    rotary.setBoundaries(DEFAULT_MIN_ACCEL_Y, DEFAULT_MAX_ACCEL_Y);
    rotary.setPosition(accel);
    break;

  case Axis::Z:
    setAccelerationText->setText("Set Z mm/s^2");
    setAccelerationValue->setText(newText);
    rotary.setBoundaries(DEFAULT_MIN_ACCEL_Z, DEFAULT_MAX_ACCEL_Z);
    rotary.setPosition(accel);
    break;
  }

  setAccelerationText->setColumn(ACCEL_LBL_COL);
  setAccelerationValue->setColumn(ACCEL_VAL_COL);

  display(ScreenID::SET_ACCEL, false);
}

void displayMoveAxisScreen()
{
  char newField[MOVE_AXES_VAL_SIZE];

  Axis axis = cartesian.getCurrentAxis();
  int pos = cartesian.getTargetPosition(axis);
  Unit unit = cartesian.getUnit();

  std::string newText = "Move";

  switch (axis)
  {
  case Axis::X:
    newText.append(" X");
    break;

  case Axis::Y:
    newText.append(" Y");
    break;

  case Axis::Z:
    newText.append(" Z");
    break;
  }

  snprintf(newField, MOVE_AXES_VAL_SIZE, "%.1f", pos / 1000);

  switch (unit)
  {
  case Unit::CENTIMETER:
    newText.append(" 10mm");
    break;
  case Unit::MICROMETER:
    newText.append(" 0.1mm");
    break;
  case Unit::MILLIMETER:
    newText.append(" 1mm");
    break;
  }

  moveAxisText->setText(newText.c_str());
  moveAxisValue->setText(newField);

  moveAxisText->setColumn(MOVE_LBL_COL);
  moveAxisValue->setColumn(MOVE_VAL_COL);

  rotary.setBoundaries(INT16_MIN, INT16_MAX);
  rotary.setPosition(pos);
  rotary.setInterval(100);

  display(ScreenID::MOVE_AXIS, false);
}

void displayLaserScreen()
{
  uint8_t maxPower = laser.getMaxPower();
  String newText = String(maxPower);

  setPowerText->setText("Set Laser PWM");
  setPowerValue->setColumn(LSR_LBL_COL);

  setPowerText->setText(newText);
  setPowerValue->setColumn(LSR_VAL_COL);

  rotary.setBoundaries(LSR_MIN_POWER, LSR_MAX_POWER);
  rotary.setPosition(maxPower);

  display(ScreenID::SET_POWER, false);
}

void displayStepsScreen()
{
  Axis axis = cartesian.getCurrentAxis();
  long stepsPer = cartesian.getStepsPerMillimeter(axis);
  String newText = String(stepsPer);

  switch (axis)
  {
  case Axis::X:
    setStepsText->setText("Set X Steps/mm");
    setStepsValue->setText(newText);
    rotary.setBoundaries(DEFAULT_MIN_STEPS_X, DEFAULT_MAX_STEPS_X);
    rotary.setPosition(stepsPer);
    break;

  case Axis::Y:
    setStepsText->setText("Set Y Steps/mm");
    setStepsValue->setText(newText);
    rotary.setBoundaries(DEFAULT_MIN_STEPS_Y, DEFAULT_MAX_STEPS_Y);
    rotary.setPosition(stepsPer);
    break;

  case Axis::Z:
    setStepsText->setText("Set Z Steps/mm");
    setStepsValue->setText(newText);
    rotary.setBoundaries(DEFAULT_MIN_STEPS_Z, DEFAULT_MAX_STEPS_Z);
    rotary.setPosition(stepsPer);
    break;
  }

  setStepsText->setColumn(STEPS_LBL_COL);
  setStepsValue->setColumn(STEPS_VAL_COL);

  display(ScreenID::SET_STEPS, false);
}

void displayVelocityScreen()
{
  Axis axis = cartesian.getCurrentAxis();
  int maxSpeed = cartesian.getMaxSpeed(axis);
  String newText = String(maxSpeed);

  switch (axis)
  {
  case Axis::X:
    setVelocityText->setText("Set X mm/s");
    setVelocityValue->setText(newText);
    rotary.setBoundaries(DEFAULT_MIN_STEPS_X, DEFAULT_MAX_SPEED_X);
    rotary.setPosition(maxSpeed);
    break;

  case Axis::Y:
    setVelocityText->setText("Set Y mm/s");
    setVelocityValue->setText(newText);
    rotary.setBoundaries(DEFAULT_MIN_STEPS_Y, DEFAULT_MAX_SPEED_Y);
    rotary.setPosition(maxSpeed);
    break;

  case Axis::Z:
    setVelocityText->setText("Set Z mm/s");
    setVelocityValue->setText(newText);
    rotary.setBoundaries(DEFAULT_MIN_STEPS_Z, DEFAULT_MAX_SPEED_Z);
    rotary.setPosition(maxSpeed);
    break;
  }

  setVelocityText->setColumn(VELOCITY_LBL_COL);
  setVelocityValue->setColumn(VELOCITY_VAL_COL);

  display(ScreenID::SET_VEL, false);
}

void autohome()
{
  commands.push(std::make_shared<AutohomeCommand>(cartesian, laser));
}

void linearMove(float x, float y, float z, float feedRate, uint8_t power)
{
  commands.push(std::make_shared<LinearMoveCommand>(cartesian, laser, x, y, z, feedRate, power));
}

void displayChar(byte charValue[])
{
  lcd.createChar(LCD_ICON_CHAR, charValue);
  processIconValue->setSymbol(LCD_ICON_CHAR);
  processIconValue->displayAsChar(lcd);
}

void pauseProcess()
{
  MAIN_MENU_SCREEN.unhide(MainOption::RESUME_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainOption::PAUSE_PROC_OP);

  proc->pause();
  displayInfoScreen();
  displayChar(PAUSE_CHAR);
}

void resumeProcess()
{
  MAIN_MENU_SCREEN.unhide(MainOption::PAUSE_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainOption::RESUME_PROC_OP);

  proc->resume();
  displayInfoScreen();
  displayChar(PLAY_CHAR);
}

void stopProcess()
{
  MAIN_MENU_SCREEN.hide(MainOption::RESUME_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainOption::PAUSE_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainOption::STOP_PROC_OP);

  proc->stop();
  displayInfoScreen();
  displayChar(STOP_CHAR);
}

bool isGcodeFile(String filename)
{
  String ext = filename.substring(filename.indexOf("."));
  ext.toLowerCase();
  return ext == GCODE_FILE_EXT;
}

bool isHiddenFile(String filename)
{
  return filename.indexOf(INTERNAL_FILE_EXT) > 0 || filename == SYSTEM_VOLUME;
}

uint8_t getFiles(File &root, String paths[], uint8_t maxFilePaths)
{
  File file;
  uint8_t i;
  String path;

  while (i < maxFilePaths)
  {
    file = root.openNextFile();

    if (!file)
      break;

    path = file.name();

    if (isHiddenFile(path) || file.isDirectory())
    {
      file.close();
      continue;
    }

    if (isGcodeFile(path))
    {
      file.close();
      paths[i] = path;
      i++;
    }
  }

  return i;
}

void setHomeOffsets()
{
  cartesian.stopSteppers();

  float x = cartesian.toUnit(Axis::X, Unit::MILLIMETER);
  float y = cartesian.toUnit(Axis::Y, Unit::MILLIMETER);
  float z = cartesian.toUnit(Axis::Z, Unit::MILLIMETER);

  cartesian.setHomeOffset(Axis::X, x);
  cartesian.setHomeOffset(Axis::Y, y);
  cartesian.setHomeOffset(Axis::Z, z);
}

void moveAxis(Rotation direction)
{
  unsigned long u = cartesian.getUnit() == Unit::MICROMETER ? 100 : 1; // move 0.1mm if unit micron

  if (direction == Rotation::COUNTERCLOCKWISE)
  {
    u = -u;
  }

  switch (cartesian.getCurrentAxis())
  {
  case Axis::X:
    linearMove(u, 0, 0, FEED_RATE_MOVE_AXIS, 0);
    break;
  case Axis::Y:
    linearMove(0, u, 0, FEED_RATE_MOVE_AXIS, 0);
    break;
  case Axis::Z:
    linearMove(0, 0, u, FEED_RATE_MOVE_AXIS, 0);
  }

  displayMoveAxisScreen();
}

void setAcceleration()
{
  Axis axis = cartesian.getCurrentAxis();
  long pos = rotary.getPosition();

  cartesian.setAcceleration(axis, Unit::MILLIMETER, pos);

  displayAccelerationScreen();
}

void setPower(Rotation rotation)
{
  Axis axis = cartesian.getCurrentAxis();
  long pos = rotary.getPosition();

  laser.setMaxPower(pos);
  displayLaserScreen();
}

void setSteps(Rotation rotation)
{
  Axis axis = cartesian.getCurrentAxis();
  long pos = rotary.getPosition();

  cartesian.setStepsPerMillimeter(axis, pos);
  displayStepsScreen();
}

void setVelocity(Rotation rotation)
{
  Axis axis = cartesian.getCurrentAxis();
  long pos = rotary.getPosition();

  cartesian.setStepsPerMillimeter(axis, pos);
  displayVelocityScreen();
}

void aboutScreenClicked()
{
  displayInfoScreen();
}

void accelerationScreenClicked()
{
  switch (ACCEL_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::MOTION, true);
    break;
  case 1:
    cartesian.setCurrentAxis(Axis::X);
    displayAccelerationScreen();
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Y);
    displayAccelerationScreen();
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Z);
    displayAccelerationScreen();
    break;
  default:
    break;
  }
}

void storeSettings()
{
  displayInfoScreen();
}

void cardScreenClicked()
{
  uint8_t index = CARD_MENU_SCREEN.getCurrentLineIndex();

  if (index > 0)
  {
    processes.push(std::make_shared<Process>(SD, rtc, "/", filePaths[index - 1]));
    cartesian.enableSteppers();

    MAIN_MENU_SCREEN.unhide(MainOption::PAUSE_PROC_OP);
    MAIN_MENU_SCREEN.unhide(MainOption::STOP_PROC_OP);
    MAIN_MENU_SCREEN.hide(MainOption::PREP_MENU_RTN);
    MAIN_MENU_SCREEN.hide(MainOption::START_FROM_TF_OP);
    MAIN_MENU_SCREEN.hide(MainOption::RESUME_PROC_OP);

    displayInfoScreen();
  }
  else
  {
    displayMenu(ScreenID::MAIN, true);
  }
}

void ctrlScreenClicked()
{
  switch (CTRL_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::MAIN, true);
    break;
  case 1:
    displayMenu(ScreenID::MOTION, false);
    break;
  case 2:
    displayLaserScreen();
    break;
  case 3:
    storeSettings();
    break;
  default:
    break;
  }
}

void infoScreenClicked()
{
  display(ScreenID::MAIN, false);
}

void mainScreenClicked()
{
  switch (MAIN_MENU_SCREEN.getCurrentLineIndex())
  {
  case MainOption::INFO_MENU_RTN:
    displayInfoScreen();
    break;

  case MainOption::PREP_MENU_RTN:
    displayMenu(ScreenID::PREP, false);
    break;

  case MainOption::CTRL_MENU_RTN:
    displayMenu(ScreenID::CTRL, false);
    break;

  case MainOption::NO_TF_CARD_OP:
    displayInfoScreen();
    break;

  case MainOption::START_FROM_TF_OP:
    displayMenu(ScreenID::CARD, false);
    break;

  case MainOption::PAUSE_PROC_OP:
    pauseProcess();
    break;

  case MainOption::RESUME_PROC_OP:
    resumeProcess();
    break;

  case MainOption::STOP_PROC_OP:
    stopProcess();
    break;

  case MainOption::ABOUT_CNC_OP:
    display(ScreenID::ABOUT, false);
    break;

  default:
    break;
  }
}

void motionScreenClicked()
{
  switch (MOTION_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::CTRL, true);
    break;
  case 1:
    displayMenu(ScreenID::ACCEL, false);
    break;
  case 2:
    displayMenu(ScreenID::VEL, false);
    break;
  case 3:
    displayMenu(ScreenID::STEPS, false);
    break;
  default:
    break;
  }
}

void moveAxesScreenClicked()
{
  switch (MOVE_AXES_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::PREP, true);
    break;
  case 1:
    displayMenu(ScreenID::MOVE_X, false);
    break;
  case 2:
    displayMenu(ScreenID::MOVE_Y, false);
    break;
  case 3:
    displayMenu(ScreenID::MOVE_Z, false);
    break;
  default:
    break;
  }
}

void moveAxisScreenClicked()
{
  Axis axis = cartesian.getCurrentAxis();

  switch (axis)
  {
  case Axis::X:
    display(ScreenID::MOVE_X, true);
    break;
  case Axis::Y:
    display(ScreenID::MOVE_Y, true);
    break;
  case Axis::Z:
    display(ScreenID::MOVE_Z, true);
    break;
  }
}

void moveXScreenClicked()
{
  switch (MOVE_X_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::MOVE_AXES, true);
    cartesian.setCurrentAxis(Axis::X);
    cartesian.setUnit(Unit::MILLIMETER);
    break;
  case 1:
    cartesian.setCurrentAxis(Axis::X);
    cartesian.setUnit(Unit::CENTIMETER);
    displayMoveAxisScreen();
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::X);
    cartesian.setUnit(Unit::MILLIMETER);
    displayMoveAxisScreen();
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::X);
    cartesian.setUnit(Unit::MICROMETER);
    displayMoveAxisScreen();
    break;
  default:
    break;
  }
}

void moveYScreenClicked()
{
  switch (MOVE_Y_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::MOVE_AXES, true);
    cartesian.setCurrentAxis(Axis::X);
    cartesian.setUnit(Unit::MILLIMETER);
    break;
  case 1:
    cartesian.setCurrentAxis(Axis::Y);
    cartesian.setUnit(Unit::CENTIMETER);
    displayMoveAxisScreen();
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Y);
    cartesian.setUnit(Unit::MILLIMETER);
    displayMoveAxisScreen();
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Y);
    cartesian.setUnit(Unit::MICROMETER);
    displayMoveAxisScreen();
    break;
  default:
    break;
  }
}

void moveZScreenClicked()
{
  switch (MOVE_Z_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::MOVE_AXES, true);
    cartesian.setCurrentAxis(Axis::X);
    cartesian.setUnit(Unit::MILLIMETER);
    break;
  case 1:
    cartesian.setCurrentAxis(Axis::Z);
    cartesian.setUnit(Unit::CENTIMETER);
    displayMoveAxisScreen();
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Z);
    cartesian.setUnit(Unit::MILLIMETER);
    displayMoveAxisScreen();
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Z);
    cartesian.setUnit(Unit::MICROMETER);
    displayMoveAxisScreen();
    break;
  default:
    break;
  }
}

void prepScreenClicked()
{
  switch (PREP_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::MAIN, true);
    break;
  case 1:
    displayMenu(ScreenID::MOVE_AXES, false);
    break;
  case 2:
    autohome();
    displayInfoScreen();
    break;
  case 3:
    setHomeOffsets();
    displayInfoScreen();
    break;
  case 4:
    cartesian.disableSteppers();
    displayInfoScreen();
    break;
  default:
    break;
  }
}

void setAccelScreenClicked()
{
  rotary.setBoundaries(0, 1);
  rotary.setInterval(1);
  rotary.setPosition(0);

  display(ScreenID::MOTION, true);
}

void setPowerScreenClicked()
{
  rotary.setBoundaries(0, 1);
  rotary.setInterval(1);
  rotary.setPosition(0);

  display(ScreenID::CTRL, true);
}

void setStepsScreenClicked()
{
  rotary.setBoundaries(0, 1);
  rotary.setInterval(1);
  rotary.setPosition(0);

  display(ScreenID::MOTION, true);
}

void setVelScreenClicked()
{
  rotary.setBoundaries(0, 1);
  rotary.setInterval(1);
  rotary.setPosition(0);

  display(ScreenID::MOTION, true);
}

void stepsScreenClicked()
{
  switch (STEPS_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::MOTION, true);
    break;
  case 1:
    cartesian.setCurrentAxis(Axis::X);
    displayStepsScreen();
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Y);
    displayStepsScreen();
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Z);
    displayStepsScreen();
    break;
  default:
    break;
  }
}

void velocityScreenClicked()
{
  switch (VEL_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::MOTION, true);
    break;
  case 1:
    cartesian.setCurrentAxis(Axis::X);
    displayVelocityScreen();
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Y);
    displayVelocityScreen();
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Z);
    displayVelocityScreen();
    break;
  default:
    break;
  }
}

void clickCallback()
{
  switch (currentScreen)
  {
  case ScreenID::ABOUT:
    aboutScreenClicked();
    break;
  case ScreenID::ACCEL:
    accelerationScreenClicked();
    break;
  case ScreenID::CARD:
    cardScreenClicked();
    break;
  case ScreenID::CTRL:
    ctrlScreenClicked();
    break;
  case ScreenID::INFO:
    infoScreenClicked();
    break;
  case ScreenID::MAIN:
    mainScreenClicked();
    break;
  case ScreenID::MOTION:
    motionScreenClicked();
    break;
  case ScreenID::MOVE_AXES:
    moveAxesScreenClicked();
    break;
  case ScreenID::MOVE_AXIS:
    moveAxisScreenClicked();
    break;
  case ScreenID::MOVE_X:
    moveXScreenClicked();
    break;
  case ScreenID::MOVE_Y:
    moveYScreenClicked();
    break;
  case ScreenID::MOVE_Z:
    moveZScreenClicked();
    break;
  case ScreenID::PREP:
    prepScreenClicked();
    break;
  case ScreenID::SET_ACCEL:
    setAccelScreenClicked();
    break;
  case ScreenID::SET_POWER:
    setPowerScreenClicked();
    break;
  case ScreenID::SET_STEPS:
    setStepsScreenClicked();
    break;
  case ScreenID::SET_VEL:
    setVelScreenClicked();
    break;
    break;
  case ScreenID::STEPS:
    stepsScreenClicked();
    break;
  case ScreenID::VEL:
    velocityScreenClicked();
    break;
  default:
    break;
  }
}

void rotateCallback(Rotation rotation)
{
  LiquidScreen *screen = getScreen(currentScreen);

  if (screen)
  {
    switch (currentScreen)
    {
    case ScreenID::INFO:
      break;
    case ScreenID::MOVE_AXIS:
      moveAxis(rotation);
      break;
    case ScreenID::SET_ACCEL:
      setAcceleration();
      break;
    case ScreenID::SET_POWER:
      setPower(rotation);
      break;
    case ScreenID::SET_STEPS:
      setSteps(rotation);
      break;
    case ScreenID::SET_VEL:
      setVelocity(rotation);
      break;
    default:
      if (rotation == Rotation::CLOCKWISE)
      {
        screen->nextLine();
      }
      else
      {
        screen->previousLine();
      }

      screen->display(false);
      break;
    }
  }
}

float parseNumber(String line, int index, char arg, float val)
{
  if (index < 0)
  {
    return val;
  }

  String sub = line.substring(index + 1);
  index = sub.indexOf(' ');
  sub = sub.substring(0, index);

  return atof(sub.c_str());
}

float parseNumber(String line, char arg, float val)
{
  return parseNumber(line, line.indexOf(arg), arg, val);
}

LiquidLinePtr createLine(LiquidScreen &screen, uint8_t lineIndex, uint8_t col, uint8_t row, String text, size_t textLength)
{
  LiquidLinePtr ptr = std::make_shared<LiquidLine>(col, row, pad_right(text.c_str(), textLength).c_str());

  screen.append(lineIndex, ptr);
  return ptr;
}

void setupAboutScreen()
{
  createLine(ABOUT_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Main", 4);
}

void setupAccelerationScreen()
{
  createLine(ACCEL_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Motion", 8);
  createLine(ACCEL_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "Accel X", 8);
  createLine(ACCEL_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Accel Y", 8);
  createLine(ACCEL_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Accel Z", 8);
}

void setupCardScreen()
{
  createLine(CARD_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Main", 4);

  uint8_t n = getFiles(root, filePaths, MAX_FILES_SIZE);

  for (uint8_t i = 0; i < n; i++)
  {
    createLine(CARD_MENU_SCREEN, i + 1, LCD_MENU_COL, LCD_ZERO_ROW, filePaths[i].c_str(), 18);
  }
}

void setupCtrlScreen()
{
  createLine(CTRL_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Main", 15);
  createLine(CTRL_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "Motion", 15);
  createLine(CTRL_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Laser", 15);
  createLine(CTRL_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Store settings", 15);
}

void setupInfoScreen()
{
  createLine(INFO_SCREEN, 0, LCD_ZERO_COL, 0, "Boreal CNC", 0);
  createLine(INFO_SCREEN, 1, POS_LBL_X_COL, 1, "X", 0);
  createLine(INFO_SCREEN, 2, POS_LBL_Y_COL, 1, "Y", 0);
  createLine(INFO_SCREEN, 3, POS_LBL_Z_COL, 1, "Z", 0);

  positionValueX = createLine(INFO_SCREEN, 4, POS_VAL_X_COL, 1, "0", 0);
  positionValueY = createLine(INFO_SCREEN, 5, POS_VAL_Y_COL, 1, "0", 0);
  positionValueZ = createLine(INFO_SCREEN, 6, POS_VAL_Z_COL, 1, "0", 0);

  processIconValue = createLine(INFO_SCREEN, 7, PROC_ICON_VAL_COL, 2, " ", 0);
  processNameValue = createLine(INFO_SCREEN, 8, PROC_NAME_LBL_COL, 2, "Ready!", 0);
  processTimeValue = createLine(INFO_SCREEN, 9, PROC_TIME_LBL_COL, 2, "00:00", 0);

  progressBar = createLine(INFO_SCREEN, 10, PROG_BAR_COL, 3, "", 0);
  progressValue = createLine(INFO_SCREEN, 11, PROG_VAL_COL, 3, "0%", 0);
}

void setupMainScreen()
{
  createLine(MAIN_MENU_SCREEN, MainOption::INFO_MENU_RTN, LCD_MENU_COL, LCD_ZERO_ROW, "Info", 14);
  createLine(MAIN_MENU_SCREEN, MainOption::PREP_MENU_RTN, LCD_MENU_COL, LCD_ZERO_ROW, "Prepare", 14);
  createLine(MAIN_MENU_SCREEN, MainOption::CTRL_MENU_RTN, LCD_MENU_COL, LCD_ZERO_ROW, "Control", 14);
  createLine(MAIN_MENU_SCREEN, MainOption::NO_TF_CARD_OP, LCD_MENU_COL, LCD_ZERO_ROW, "No TF Card", 14);
  createLine(MAIN_MENU_SCREEN, MainOption::START_FROM_TF_OP, LCD_MENU_COL, LCD_ZERO_ROW, "Start from TF", 14);
  createLine(MAIN_MENU_SCREEN, MainOption::PAUSE_PROC_OP, LCD_MENU_COL, LCD_ZERO_ROW, "Pause", 14);
  createLine(MAIN_MENU_SCREEN, MainOption::RESUME_PROC_OP, LCD_MENU_COL, LCD_ZERO_ROW, "Resume", 14);
  createLine(MAIN_MENU_SCREEN, MainOption::STOP_PROC_OP, LCD_MENU_COL, LCD_ZERO_ROW, "Stop", 14);
  createLine(MAIN_MENU_SCREEN, MainOption::ABOUT_CNC_OP, LCD_MENU_COL, LCD_ZERO_ROW, "About CNC", 14);

  MAIN_MENU_SCREEN.hide(MainOption::NO_TF_CARD_OP);
  MAIN_MENU_SCREEN.hide(MainOption::PAUSE_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainOption::RESUME_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainOption::STOP_PROC_OP);
}

void setupMoveAxisScreen()
{
  moveAxisText = createLine(MOVE_AXIS_SCREEN, 0, LCD_ZERO_COL, 1, "Move X 1mm", 0);
  moveAxisValue = createLine(MOVE_AXIS_SCREEN, 1, LCD_ZERO_COL, 2, "0", 0);
}

void setupMoveAxesScreen()
{
  createLine(MOVE_AXES_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Prepare", 8);
  createLine(MOVE_AXES_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "Move X", 8);
  createLine(MOVE_AXES_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Move Y", 8);
  createLine(MOVE_AXES_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Move Z", 8);
}

void setupMoveXScreen()
{
  createLine(MOVE_X_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Move Axes", 13);
  createLine(MOVE_X_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "Move X 10mm", 13);
  createLine(MOVE_X_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Move X 1mm", 13);
  createLine(MOVE_X_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Move X 0.1mm", 13);
}

void setupMoveYScreen()
{

  createLine(MOVE_Y_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Move Axes", 13);
  createLine(MOVE_Y_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "Move Y 10mm", 13);
  createLine(MOVE_Y_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Move Y 1mm", 13);
  createLine(MOVE_Y_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Move Y 0.1mm", 13);
}

void setupMoveZScreen()
{
  createLine(MOVE_Z_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Move Axes", 13);
  createLine(MOVE_Z_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "Move Z 10mm", 13);
  createLine(MOVE_Z_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Move Z 1mm", 13);
  createLine(MOVE_Z_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Move Z 0.1mm", 13);
}

void setupPrepScreen()
{
  createLine(PREP_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Main", 17);
  createLine(PREP_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "Move axes", 17);
  createLine(PREP_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Auto home", 17);
  createLine(PREP_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Set home offsets", 17);
  createLine(PREP_MENU_SCREEN, 4, LCD_MENU_COL, LCD_ZERO_ROW, "Disable steppers", 17);
}

void setupMotionScreen()
{
  createLine(MOTION_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Control", 13);
  createLine(MOTION_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "Acceleration", 13);
  createLine(MOTION_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Velocity", 13);
  createLine(MOTION_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Steps/mm", 13);
}

void setupSetAccelerationScreen()
{
  setAccelerationText = createLine(SET_ACCEL_SCREEN, 0, LCD_ZERO_COL, 1, "Set X Accel", 0);
  setAccelerationValue = createLine(SET_ACCEL_SCREEN, 1, LCD_ZERO_COL, 2, "0", 0);
}

void setupSetPowerScreen()
{
  setPowerText = createLine(SET_POWER_SCREEN, 0, LCD_ZERO_COL, 1, "Set Laser PWM", 0);
  setPowerValue = createLine(SET_POWER_SCREEN, 1, LCD_ZERO_COL, 2, "0", 0);
}

void setupSetStepsScreen()
{
  setStepsText = createLine(SET_STEPS_SCREEN, 0, LCD_ZERO_COL, 1, "Set X steps/mm", 0);
  setStepsValue = createLine(SET_STEPS_SCREEN, 1, LCD_ZERO_COL, 2, "0", 0);
}

void setupSetVelocityScreen()
{
  setVelocityText = createLine(SET_VEL_SCREEN, 0, LCD_ZERO_COL, 1, "Set X Velocity", 0);
  setVelocityValue = createLine(SET_VEL_SCREEN, 1, LCD_ZERO_COL, 2, "0", 0);
}

void setupStepsScreen()
{
  createLine(STEPS_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Motion", 11);
  createLine(STEPS_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "X steps/mm", 11);
  createLine(STEPS_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Y steps/mm", 11);
  createLine(STEPS_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Z steps/mm", 11);
}

void setupVelocityScreen()
{
  createLine(ACCEL_MENU_SCREEN, 0, LCD_MENU_COL, LCD_ZERO_ROW, "Motion", 7);
  createLine(ACCEL_MENU_SCREEN, 1, LCD_MENU_COL, LCD_ZERO_ROW, "Vel X", 7);
  createLine(ACCEL_MENU_SCREEN, 2, LCD_MENU_COL, LCD_ZERO_ROW, "Vel Y", 7);
  createLine(ACCEL_MENU_SCREEN, 3, LCD_MENU_COL, LCD_ZERO_ROW, "Vel Z", 7);
}

void parseNextCommand()
{
  if (commands.size() >= CMD_QUEUE_SIZE)
  {
    return;
  }

  String line = proc->readNextLine();

  if (line.isEmpty())
  {
    return;
  }

  int code = parseNumber(line, 'G', -1);

  switch (code)
  {
  case 0:
  case 1:
    linearMove(parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'F', 0), parseNumber(line, 'S', 0));
    break;
  case 2:
  case 3:
    if (line.indexOf('I') > 0 || line.indexOf('J') > 0)
    {
      // TODO: invalid arc
      if (line.indexOf('R') > 0)
      {
        return;
      }

      commands.push(std::make_shared<ArcMoveCommand>(cartesian, laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'I', 0), parseNumber(line, 'J', 0), parseNumber(line, 'K', 0), parseNumber(line, 'F', 0), parseNumber(line, 'S', 0)));
    }
    else
    {
      // TODO: invalid circle
      if (line.indexOf('X') < 0 && line.indexOf('Y') < 0)
      {
        return;
      }

      commands.push(std::make_shared<CircleMoveCommand>(cartesian, laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'R', 0), parseNumber(line, 'F', 0), parseNumber(line, 'S', 0)));
    }

    break;
  case 4:
    if (line.indexOf('S') > 0)
    {
      commands.push(std::make_shared<DwellCommand>(parseNumber(line, 'S', 0) * 1000));
    }
    else
    {
      commands.push(std::make_shared<DwellCommand>(parseNumber(line, 'P', 0)));
    }
    break;

  case 28:
    autohome();
    break;
  default:
    break;
  }
}

void onCompleted()
{
  if (commands.empty())
  {
    cartesian.disableSteppers();
    proc = nullptr;

    displayInfoScreen();
  }
}

void onContinue()
{
  if (cmd && cmd->status() == Status::STOPPED)
  {
    cmd->setup();
  }
  else
  {
    parseNextCommand();

    if (!commands.empty() && !cmd)
    {
      cmd = commands.front();
      commands.pop();

      if (cmd)
      {
        cmd->setup();
      }
    }
  }

  if (cmd)
  {
    Status status = cmd->status();

    switch (status)
    {
    case Status::COMPLETED:
      cmd = nullptr;
      break;

    case Status::CONTINUE:
      cmd->execute();
      break;

    case Status::ERROR:
      cmd = nullptr;
      break;

    default:
      break;
    }
  }
}

void onError()
{
  proc = nullptr;
}

void onPaused()
{
  cmd->stop();
}

void onStopped()
{
  cmd->stop();

  if (millis() - proc->getLastStopTime() > PROCESS_EXPIRED)
  {
    commands = {};
    cmd = nullptr;
    proc = nullptr;

    MAIN_MENU_SCREEN.unhide(MainOption::PREP_MENU_RTN);
    MAIN_MENU_SCREEN.unhide(MainOption::START_FROM_TF_OP);

    displayInfoScreen();
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(CS_RS1_PIN, OUTPUT);
  pinMode(CS_RS2_PIN, OUTPUT);
  pinMode(CS_RS3_PIN, OUTPUT);
  pinMode(CS_RS4_PIN, OUTPUT);

  pinMode(EN_CLK_PIN, INPUT);
  pinMode(EN_DT_PIN, INPUT);
  pinMode(EN_SW_PIN, INPUT_PULLUP);

  pinMode(SDA, PULLUP);
  pinMode(SCL, PULLUP);

  spi.begin();

  Serial.print("SD ");

  if (!SD.begin(SS))
  {
    Serial.println("failed to mount!");
    return;
  }

  Serial.println("mount!");

  Wire.begin();
  Wire.setClock(800000);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.createChar(LCD_ICON_CHAR, ARROW_CHAR);
  lcd.createChar(LCD_GAUGE_FILL_1, GAUGE_FILL_1);
  lcd.createChar(LCD_GAUGE_FILL_2, GAUGE_FILL_2);
  lcd.createChar(LCD_GAUGE_FILL_3, GAUGE_FILL_3);
  lcd.createChar(LCD_GAUGE_FILL_4, GAUGE_FILL_4);
  lcd.createChar(LCD_GAUGE_LEFT, GAUGE_LEFT);
  lcd.createChar(LCD_GAUGE_RIGHT, GAUGE_RIGHT);
  lcd.createChar(LCD_GAUGE_EMPTY, GAUGE_EMPTY);

  root = SD.open("/");

  cartesian.setAcceleration(Axis::X, Unit::MILLIMETER, DEFAULT_MIN_ACCEL_X);
  cartesian.setAcceleration(Axis::Y, Unit::MILLIMETER, DEFAULT_MIN_ACCEL_Y);
  cartesian.setAcceleration(Axis::Z, Unit::MILLIMETER, DEFAULT_MIN_ACCEL_Z);

  SWITCH_X->setDebounceTime(DEBOUNCE_TIME);
  SWITCH_Y->setDebounceTime(DEBOUNCE_TIME);
  SWITCH_Z->setDebounceTime(DEBOUNCE_TIME);

  cartesian.setMaxSpeed(Axis::X, Unit::MILLIMETER, DEFAULT_MAX_SPEED_X);
  cartesian.setMaxSpeed(Axis::Y, Unit::MILLIMETER, DEFAULT_MAX_SPEED_Y);
  cartesian.setMaxSpeed(Axis::Z, Unit::MILLIMETER, DEFAULT_MAX_SPEED_Z);

  laser.setMaxPower(LSR_MAX_POWER);

  rotary.setOnClicked(clickCallback);
  rotary.setOnRotation(rotateCallback);

  rotary.setClickDebounceTime(EN_CLICK_DEBOUNCE);
  rotary.setRotationDebounceTime(EN_ROT_DEBOUNCE);

  cartesian.setStepperMotor(Axis::X, STEPPER_X);
  cartesian.setStepperMotor(Axis::Y, STEPPER_Y);
  cartesian.setStepperMotor(Axis::Z, STEPPER_Z);

  cartesian.setLimitSwitch(Axis::X, SWITCH_X);
  cartesian.setLimitSwitch(Axis::Y, SWITCH_Y);
  cartesian.setLimitSwitch(Axis::Z, SWITCH_Z);

  cartesian.setMinStepsPerMillimeter(Axis::X, DEFAULT_MIN_STEPS_X);
  cartesian.setMinStepsPerMillimeter(Axis::Y, DEFAULT_MIN_STEPS_Y);
  cartesian.setMinStepsPerMillimeter(Axis::Z, DEFAULT_MIN_STEPS_Z);

  cartesian.setStepsPerMillimeter(Axis::X, DEFAULT_STEPS_X);
  cartesian.setStepsPerMillimeter(Axis::Y, DEFAULT_STEPS_Y);
  cartesian.setStepsPerMillimeter(Axis::Z, DEFAULT_STEPS_Z);

  cartesian.setDimension(Axis::X, DIMENSIONS_X);
  cartesian.setDimension(Axis::Y, DIMENSIONS_Y);
  cartesian.setDimension(Axis::Z, DIMENSIONS_Z);

  setupAboutScreen();
  setupAccelerationScreen();
  setupCardScreen();
  setupCtrlScreen();
  setupInfoScreen();
  setupMainScreen();
  setupMotionScreen();
  setupMoveAxesScreen();
  setupMoveAxisScreen();
  setupMoveXScreen();
  setupMoveYScreen();
  setupMoveZScreen();
  setupPrepScreen();
  setupSetAccelerationScreen();
  setupSetPowerScreen();
  setupSetStepsScreen();
  setupSetVelocityScreen();
  setupStepsScreen();
  setupVelocityScreen();

  cartesian.disableSteppers();
  displayInfoScreen();
}

void loop()
{
  rotary.tick();

  if (!processes.empty() && !proc)
  {
    proc = processes.front();
    processes.pop();

    if (proc)
    {
      proc->setup();

      processNameValue->setText(proc->getName());
      processNameValue->display(lcd);
    }
  }

  if (proc)
  {
    updateInfoScreen();

    switch (proc->status())
    {
    case Status::COMPLETED:
      onCompleted();
      break;

    case Status::CONTINUE:
      onContinue();
      break;

    case Status::ERROR:
      onError();
      break;

    case Status::PAUSED:
      onPaused();
      break;

    case Status::STOPPED:
      onStopped();
      break;

    default:
      break;
    }
  }
}
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include <string.h>

#include <Command.h>
#include <LimitSwitch.h>
#include <LiquidScreen.h>
#include <Process.h>
#include <Rotary.h>

#define LCD_ICON_CHAR 0
#define LCD_GAUGE_FILL_1 1
#define LCD_GAUGE_FILL_2 2
#define LCD_GAUGE_FILL_3 3
#define LCD_GAUGE_FILL_4 4
#define LCD_GAUGE_LEFT 5
#define LCD_GAUGE_RIGHT 6
#define LCD_GAUGE_EMPTY 7

#define CMD_QUEUE_SIZE 10
#define MAX_FILES_SIZE 11
#define FILE_PATH_SIZE 14

#define EN_CLICK_DEBOUNCE 800
#define EN_ROT_DEBOUNCE 25

#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

#define ICON_CHAR 0

#define FIRST_COL 0
#define SECOND_COL 1
#define THIRD_COL 2
#define FOURTH_COL 3
#define FIFTH_COL 4
#define SIXTH_COL 5

#define FIRST_ROW 0
#define SECOND_ROW 1
#define THIRD_ROW 2
#define FOURTH_ROW 3

#define MENU_COL 2

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
#define PROG_VAL_COL 16

#define ACCEL_VAL_SIZE 4
#define MOVE_AXIS_VAL_SIZE 4
#define POS_VAL_SIZE 4
#define POWER_VAL_SIZE 4
#define PROC_TIME_SIZE 6
#define PROG_VAL_SIZE 5
#define STEPS_VAL_SIZE 4
#define VEL_VAL_SIZE 4

#define CS_RS1_PIN 27
#define CS_RS2_PIN 26
#define CS_RS3_PIN 25
#define CS_RS4_PIN 33

#define EN_CLK_PIN 4
#define EN_DT_PIN 2
#define EN_SW_PIN 17

#define LSR_PIN 32 // TODO: use GPIO 13

#define SW_X_PIN 36
#define SW_Y_PIN 39
#define SW_Z_PIN 34

enum Icon
{
  ARROW,
  CHECK,
  PLAY,
  PAUSE,
  STOP
};

enum InfoLine
{
  TITLE_LBL,
  POS_LBL_X,
  POS_LBL_Y,
  POS_LBL_Z,
  POS_LBL_X_VAL,
  POS_LBL_Y_VAL,
  POS_LBL_Z_VAL,
  PROC_ICON_VAL,
  PROC_NAME_LBL,
  PROC_TIME_LBL,
  PROG_BAR,
  PROG_VAL
};

enum MainLine
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

enum MoveAxisLine
{
  MOVE_AXIS_LBL,
  MOVE_AXIS_VAL
};

enum SetAccelerationLine
{
  SET_ACCELERATION_LBL,
  SET_ACCELERATION_VAL
};

enum SetPowerLine
{
  SET_POWER_LBL,
  SET_POWER_VAL
};

enum SetStepsLine
{
  SET_STEPS_LBL,
  SET_STEPS_VAL
};

enum SetVelocityLine
{
  SET_VELOCITY_LBL,
  SET_VELOCITY_VAL
};

enum ScreenID
{
  ABOUT,
  ACCEL,
  CARD,
  CTRL,
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

Laser laser(LSR_PIN);

LCD lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

Cartesian cartesian;

RTC rtc;

ShiftRegisterMotorInterface SHIFT_REGISTER_X1(SPI, CS_RS1_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_X2(SPI, CS_RS2_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Y(SPI, CS_RS3_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Z(SPI, CS_RS4_PIN, MSBFIRST);

BilateralMotorInterface BILATERAL_X(SHIFT_REGISTER_X1, SHIFT_REGISTER_X2);

StepperMotor stepperX(BILATERAL_X);
StepperMotor stepperY(SHIFT_REGISTER_Y);
StepperMotor stepperZ(SHIFT_REGISTER_Z);

LimitSwitch switchX(SW_X_PIN);
LimitSwitch switchY(SW_Y_PIN);
LimitSwitch switchZ(SW_Z_PIN);

LiquidScreen ABOUT_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen INFO_SCREEN(lcd, LCD_COLS, LCD_ROWS);

LiquidScreen MOVE_AXIS_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_ACCEL_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_POWER_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_STEPS_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_VEL_SCREEN(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MAIN_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu PREP_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CTRL_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CARD_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOVE_AXES_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_X_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Y_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Z_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu ACCEL_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu VEL_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOTION_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu STEPS_MENU_SCREEN(lcd, LCD_COLS, LCD_ROWS);

char INTERNAL_FILE_EXT = '~';

const char *EMPTY_ROW = "                    ";
const char *READY_MSG = "Ready!";
const char *SYSTEM_VOLUME = "System Volume Information";
const char *GCODE_FILE_EXT = ".gcode";

const uint16_t DEFAULT_FEED_RATE = 3000;
const uint16_t FEED_RATE_MOVE_AXIS = 1200;

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
const int PROCESS_EXPIRED = 10000;
const int CHAR_PIXEL_WIDTH = 5;
const int EMPTY_SPACE = 255;
const int GAUGE_SIZE = 14;
const int ICON_SIZE = 1;

const uint8_t ABOUT_SIZE = 4;
const uint8_t ACCEL_SIZE = 4;
const uint8_t CARD_SIZE = 12;
const uint8_t CTRL_SIZE = 3;
const uint8_t INFO_SIZE = 12;
const uint8_t MAIN_SIZE = 9;
const uint8_t MOVE_AXES_SIZE = 4;
const uint8_t MOVE_AXIS_SIZE = 2;
const uint8_t MOVE_X_SIZE = 4;
const uint8_t MOVE_Y_SIZE = 4;
const uint8_t MOVE_Z_SIZE = 4;
const uint8_t MOTION_SIZE = 4;
const uint8_t PREP_SIZE = 5;
const uint8_t SET_ACCEL_SIZE = 2;
const uint8_t SET_POWER_SIZE = 2;
const uint8_t SET_STEPS_SIZE = 2;
const uint8_t SET_VEL_SIZE = 2;
const uint8_t STEPS_SIZE = 4;
const uint8_t VEL_SIZE = 4;

LiquidLine ABOUT_LINES[ABOUT_SIZE] = {
    LiquidLine(FIRST_COL, FIRST_ROW, "Created by: "),
    LiquidLine(THIRD_COL, SECOND_ROW, "aldorobertoavilan"),
    LiquidLine(FIRST_COL, THIRD_ROW, "Github repo:"),
    LiquidLine(THIRD_COL, FOURTH_ROW, "Boreal-CNC")};

LiquidLine ACCEl_LINES[ACCEL_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Main   "),
    LiquidLine(MENU_COL, FIRST_ROW, "Accel X"),
    LiquidLine(MENU_COL, FIRST_ROW, "Accel Y"),
    LiquidLine(MENU_COL, FIRST_ROW, "Accel Z")};

LiquidLine CTRL_LINES[CTRL_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Main  "),
    LiquidLine(MENU_COL, FIRST_ROW, "Motion"),
    LiquidLine(MENU_COL, FIRST_ROW, "Laser ")};

LiquidLine CARD_LINES[CARD_SIZE]; // initialize when SD has begun

LiquidLine INFO_LINES[INFO_SIZE] = {
    LiquidLine(FIRST_COL, FIRST_ROW, "Boreal CNC"),
    LiquidLine(POS_LBL_X_COL, SECOND_ROW, "X"),
    LiquidLine(POS_LBL_Y_COL, SECOND_ROW, "Y"),
    LiquidLine(POS_LBL_Z_COL, SECOND_ROW, "Z"),
    LiquidLine(POS_VAL_X_COL, SECOND_ROW, "0"),
    LiquidLine(POS_VAL_Y_COL, SECOND_ROW, "0"),
    LiquidLine(POS_VAL_Z_COL, SECOND_ROW, "0"),
    LiquidLine(PROC_ICON_VAL_COL, THIRD_ROW, ""),
    LiquidLine(PROC_NAME_LBL_COL, THIRD_ROW, READY_MSG),
    LiquidLine(PROC_TIME_LBL_COL, THIRD_ROW, "00:00"),
    LiquidLine(PROG_BAR_COL, FOURTH_ROW, ""),
    LiquidLine(PROG_VAL_COL, FOURTH_ROW, "0%")};

LiquidLine MAIN_LINES[MAIN_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Info   "),
    LiquidLine(MENU_COL, FIRST_ROW, "Prepare"),
    LiquidLine(MENU_COL, FIRST_ROW, "Control      "),
    LiquidLine(MENU_COL, FIRST_ROW, "No TF Card   "),
    LiquidLine(MENU_COL, FIRST_ROW, "Start from TF"),
    LiquidLine(MENU_COL, FIRST_ROW, "Pause "),
    LiquidLine(MENU_COL, FIRST_ROW, "Resume"),
    LiquidLine(MENU_COL, FIRST_ROW, "Stop  "),
    LiquidLine(MENU_COL, FIRST_ROW, "About CNC     ")};

LiquidLine MOVE_AXES_LINES[MOVE_AXES_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Prepare"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move X "),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Y "),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Z ")};

LiquidLine MOVE_AXIS_LINES[MOVE_AXIS_SIZE] = {
    LiquidLine(FIFTH_COL, SECOND_ROW, "Move X 1mm"),
    LiquidLine(FIFTH_COL, THIRD_ROW, "0")};

LiquidLine MOVE_X_LINES[MOVE_X_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Move Axes"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move X 10mm"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move X 1mm"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move X 0.1mm")};

LiquidLine MOVE_Y_LINES[MOVE_Y_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Move Axes"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Y 10mm"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Y 1mm"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Y 0.1mm")};

LiquidLine MOVE_Z_LINES[MOVE_Z_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Move Axes"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Z 10mm"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Z 1mm"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Z 0.1mm")};

LiquidLine MOTION_LINES[MOTION_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Control"),
    LiquidLine(MENU_COL, FIRST_ROW, "Acceleration"),
    LiquidLine(MENU_COL, FIRST_ROW, "Velocity"),
    LiquidLine(MENU_COL, FIRST_ROW, "Steps/mm")};

LiquidLine PREP_LINES[PREP_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Main     "),
    LiquidLine(MENU_COL, FIRST_ROW, "Move axes"),
    LiquidLine(MENU_COL, FIRST_ROW, "Auto home       "),
    LiquidLine(MENU_COL, FIRST_ROW, "Set home offsets"),
    LiquidLine(MENU_COL, FIRST_ROW, "Disable steppers")};

LiquidLine SET_ACCEL_LINES[SET_ACCEL_SIZE] = {
    LiquidLine(FIFTH_COL, SECOND_ROW, "Set X Accel"),
    LiquidLine(FIFTH_COL, THIRD_ROW, "0")};

LiquidLine SET_POWER_LINES[SET_POWER_SIZE] = {
    LiquidLine(FOURTH_COL, SECOND_ROW, "Set Laser PWM"),
    LiquidLine(FOURTH_COL, THIRD_ROW, "0")};

LiquidLine SET_STEPS_LINES[SET_STEPS_SIZE] = {
    LiquidLine(FOURTH_COL, SECOND_ROW, "Set X steps/mm"),
    LiquidLine(FOURTH_COL, THIRD_ROW, "0")};

LiquidLine SET_VEL_LINES[SET_VEL_SIZE] = {
    LiquidLine(SIXTH_COL, SECOND_ROW, "Set X mm/s"),
    LiquidLine(SIXTH_COL, THIRD_ROW, "0")};

LiquidLine STEPS_LINES[STEPS_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Motion"),
    LiquidLine(MENU_COL, FIRST_ROW, "X steps/mm"),
    LiquidLine(MENU_COL, FIRST_ROW, "Y steps/mm"),
    LiquidLine(MENU_COL, FIRST_ROW, "Z steps/mm")};

LiquidLine VEL_LINES[VEL_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Motion"),
    LiquidLine(MENU_COL, FIRST_ROW, "Vel X"),
    LiquidLine(MENU_COL, FIRST_ROW, "Vel Y"),
    LiquidLine(MENU_COL, FIRST_ROW, "Vel Z")};

const byte ARROW_CHAR[8] = {B00000, B00100, B00110, B11111, B11111, B00110, B00100, B00000};
const byte CHECK_CHAR[8] = {B00000, B00001, B00011, B10110, B11100, B01000, B00000, B00000};
const byte PLAY_CHAR[8] = {B00000, B10000, B11000, B11100, B11110, B11100, B11000, B10000};
const byte PAUSE_CHAR[8] = {B00000, B11011, B11011, B11011, B11011, B11011, B11011, B00000};
const byte STOP_CHAR[8] = {B00000, B00000, B11110, B11110, B11110, B11110, B00000, B00000};

const byte GAUGE_EMPTY[8] = {B11111, B00000, B00000, B00000, B00000, B00000, B00000, B11111};
const byte GAUGE_FILL_1[8] = {B11111, B10000, B10000, B10000, B10000, B10000, B10000, B11111};
const byte GAUGE_FILL_2[8] = {B11111, B11000, B11000, B11000, B11000, B11000, B11000, B11111};
const byte GAUGE_FILL_3[8] = {B11111, B11100, B11100, B11100, B11100, B11100, B11100, B11111};
const byte GAUGE_FILL_4[8] = {B11111, B11110, B11110, B11110, B11110, B11110, B11110, B11111};
const byte GAUGE_FILL_5[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};

const byte GAUGE_LEFT[8] = {B01111, B10000, B10000, B10000, B10000, B10000, B10000, B01111};
const byte GAUGE_RIGHT[8] = {B11110, B00001, B00001, B00001, B00001, B00001, B00001, B11110};

const byte GAUGE_MASK_LEFT[8] = {B01111, B11111, B11111, B11111, B11111, B11111, B11111, B01111};
const byte GAUGE_MASK_RIGHT[8] = {B11110, B11111, B11111, B11111, B11111, B11111, B11111, B11110};

String paths[MAX_FILES_SIZE];

byte gaugeLeftDynamic[8];
byte gaugeRightDynamic[8];

char accelBuffer[ACCEL_VAL_SIZE];
char moveAxisBuffer[MOVE_AXIS_VAL_SIZE];
char powerBuffer[POWER_VAL_SIZE];
char pathBuffer[FILE_PATH_SIZE];
char processTimeBuffer[PROC_TIME_SIZE];
char positionXBuffer[POS_VAL_SIZE];
char positionYBuffer[POS_VAL_SIZE];
char positionZBuffer[POS_VAL_SIZE];
char progressBarBuffer[GAUGE_SIZE];
char progressValueBuffer[PROG_VAL_SIZE];
char stepsBuffer[STEPS_VAL_SIZE];
char velocityBuffer[VEL_VAL_SIZE];

uint8_t nFiles;
File root;

ScreenID currentScreen;
Icon currentIcon;

CommandQueue commands;
ProcessQueue processes;
CommandPtr cmd;
ProcessPtr proc;

bool isGcodeFile(const char *filename)
{
  return strstr(filename, GCODE_FILE_EXT);
}

bool isHiddenFile(const char *filename)
{
  return filename == SYSTEM_VOLUME || strchr(filename, INTERNAL_FILE_EXT);
}

void loadFromTF()
{
  File file;
  const char *path;

  while (nFiles < MAX_FILES_SIZE)
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
      paths[nFiles] = path;
      nFiles++;
    }
  }
}

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

void display(ScreenID screenIndex)
{
  LiquidScreen *screen = getScreen(screenIndex);

  if (screen)
  {
    screen->setCurrentLine(0);

    currentScreen = screenIndex;
    screen->display();
  }
}

void displayIcon(Icon icon)
{
  switch (icon)
  {
  case ARROW:
    lcd.createChar(LCD_ICON_CHAR, ARROW_CHAR);
    break;
  case CHECK:
    lcd.createChar(LCD_ICON_CHAR, CHECK_CHAR);
    break;
  case PLAY:
    lcd.createChar(LCD_ICON_CHAR, PLAY_CHAR);
    break;
  case PAUSE:
    lcd.createChar(LCD_ICON_CHAR, PAUSE_CHAR);
    break;
  case STOP:
    lcd.createChar(LCD_ICON_CHAR, STOP_CHAR);
    break;
  }

  INFO_LINES[InfoLine::PROC_ICON_VAL].setSymbol(LCD_ICON_CHAR);
  INFO_LINES[InfoLine::PROC_ICON_VAL].displaySymbol(lcd);
  currentIcon = icon;
}

void displayMenu(ScreenID screenIndex, bool forcePosition)
{
  LiquidScreen *screen = getScreen(screenIndex);

  if (screen)
  {
    lcd.createChar(LCD_ICON_CHAR, ARROW_CHAR);

    rotary.setBoundaries(0, screen->getVisibleLineCount());

    if (forcePosition)
    {
      rotary.setPosition(screen->getCurrentLineIndex());
    }
    else
    {
      screen->setCurrentLine(0);
    }

    currentScreen = screenIndex;
    screen->display();
  }
}

void clearBuffer(char *buffer, uint8_t size)
{
  memset(buffer, ' ', size);
}

void formatPosition(char *buffer, float u)
{
  Unit unit = cartesian.getUnit();

  clearBuffer(buffer, POS_VAL_SIZE);

  switch (unit)
  {
  case Unit::INCH:
    snprintf(buffer, POS_VAL_SIZE, "%.1f", u);
    break;
  default:
    snprintf(buffer, POS_VAL_SIZE, "%i", (int)u);
    break;
  }
}

void formatProcessTime(tm time)
{
  clearBuffer(processTimeBuffer, PROC_TIME_SIZE);
  strftime(processTimeBuffer, PROC_TIME_SIZE, "%H:%M", &time);
}

void formatProgressBar(uint8_t progress)
{
  float unitsPerPixel = (GAUGE_SIZE * CHAR_PIXEL_WIDTH) / 100.0;
  int valueInPixels = round(progress * unitsPerPixel);
  int moveOffset = (CHAR_PIXEL_WIDTH - 1) - ((valueInPixels - 1) % CHAR_PIXEL_WIDTH);

  for (int i = 0; i < 8; i++)
  {
    if (valueInPixels < CHAR_PIXEL_WIDTH)
    {
      gaugeLeftDynamic[i] = (GAUGE_FILL_5[i] << moveOffset) | GAUGE_LEFT[i];
    }
    else
    {
      gaugeLeftDynamic[i] = GAUGE_FILL_5[i];
    }

    gaugeLeftDynamic[i] = gaugeLeftDynamic[i] & GAUGE_MASK_LEFT[i];
  }

  for (int i = 0; i < 8; i++)
  {
    if (valueInPixels > GAUGE_SIZE * CHAR_PIXEL_WIDTH - CHAR_PIXEL_WIDTH)
    {
      gaugeRightDynamic[i] = (GAUGE_FILL_5[i] << moveOffset) | GAUGE_RIGHT[i];
    }
    else
    {
      gaugeRightDynamic[i] = GAUGE_RIGHT[i];
    }

    gaugeRightDynamic[i] = gaugeRightDynamic[i] & GAUGE_MASK_RIGHT[i];
  }

  lcd.createChar(LCD_GAUGE_LEFT, gaugeLeftDynamic);
  lcd.createChar(LCD_GAUGE_RIGHT, gaugeRightDynamic);

  for (int i = 0; i < GAUGE_SIZE; i++)
  {
    if (i == 0)
    {
      progressBarBuffer[i] = LCD_GAUGE_LEFT;
    }
    else if (i == GAUGE_SIZE - 1)
    {
      progressBarBuffer[i] = LCD_GAUGE_RIGHT;
    }
    else
    {
      if (valueInPixels <= i * CHAR_PIXEL_WIDTH)
      {
        progressBarBuffer[i] = LCD_GAUGE_EMPTY;
      }
      else if (valueInPixels > i * CHAR_PIXEL_WIDTH && valueInPixels < (i + 1) * CHAR_PIXEL_WIDTH)
      {
        progressBarBuffer[i] = CHAR_PIXEL_WIDTH - moveOffset;
      }
      else
      {
        progressBarBuffer[i] = EMPTY_SPACE;
      }
    }
  }

  progressBarBuffer[GAUGE_SIZE] = '\0';
}

void formatProgressValue(uint8_t progress)
{
  clearBuffer(progressValueBuffer, PROG_VAL_SIZE);
  snprintf(progressValueBuffer, PROG_VAL_SIZE, "%i%%", progress);
}

void displayAboutScreen()
{
  rotary.setBoundaries(0, 1);
  rotary.setPosition(0);

  display(ScreenID::ABOUT);
}

void displayInfoScreen()
{
  rotary.setBoundaries(0, 1);
  rotary.setPosition(0);

  if (proc)
  {
    float targetX = cartesian.getTargetPosition(Axis::X);
    float targetY = cartesian.getTargetPosition(Axis::Y);
    float targetZ = cartesian.getTargetPosition(Axis::Z);
    uint8_t progress = proc->getProgress();

    formatPosition(positionXBuffer, targetX);
    formatPosition(positionYBuffer, targetY);
    formatPosition(positionZBuffer, targetZ);

    formatProcessTime(proc->getTime());
    formatProgressBar(progress);
    formatProgressValue(progress);

    INFO_LINES[InfoLine::PROC_NAME_LBL].setText(proc->getName());
    INFO_LINES[InfoLine::PROC_TIME_LBL].setText(processTimeBuffer);

    INFO_LINES[InfoLine::POS_LBL_X_VAL].setText(positionXBuffer);
    INFO_LINES[InfoLine::POS_LBL_Y_VAL].setText(positionYBuffer);
    INFO_LINES[InfoLine::POS_LBL_Z_VAL].setText(positionXBuffer);

    INFO_LINES[InfoLine::PROG_BAR].setText(progressBarBuffer);
    INFO_LINES[InfoLine::PROG_VAL].setText(progressValueBuffer);

    display(ScreenID::INFO);
  }
  else
  {
    formatProgressBar(0);

    INFO_LINES[InfoLine::POS_LBL_X_VAL].setText("0");
    INFO_LINES[InfoLine::POS_LBL_Y_VAL].setText("0");
    INFO_LINES[InfoLine::POS_LBL_Z_VAL].setText("0");

    INFO_LINES[InfoLine::PROC_ICON_VAL].setText("");
    INFO_LINES[InfoLine::PROC_NAME_LBL].setText(READY_MSG);
    INFO_LINES[InfoLine::PROC_TIME_LBL].setText("00:00");

    INFO_LINES[InfoLine::PROG_BAR].setText(progressBarBuffer);
    INFO_LINES[InfoLine::PROG_VAL].setText("0%");

    display(ScreenID::INFO);
  }
}

void updateInfoScreen()
{
  if (currentScreen != ScreenID::INFO)
  {
    return;
  }

  LiquidLine *processName = &INFO_LINES[InfoLine::PROC_NAME_LBL];

  if (processName->getText() == READY_MSG)
  {
    processName->setText(proc->getName());
    processName->displayText(lcd);
  }

  float targetX = cartesian.getTargetPosition(Axis::X);

  if (cartesian.getPrevTargetPosition(Axis::X) != targetX)
  {
    LiquidLine *positionValueX = &INFO_LINES[InfoLine::POS_LBL_X_VAL];
    formatPosition(positionXBuffer, targetX);

    positionValueX->setText(positionXBuffer);
    positionValueX->displayText(lcd);
    return;
  }

  float targetY = cartesian.getTargetPosition(Axis::Y);

  if (cartesian.getPrevTargetPosition(Axis::Y) != targetY)
  {
    LiquidLine *positionValueY = &INFO_LINES[InfoLine::POS_LBL_Y_VAL];
    formatPosition(positionYBuffer, targetY);

    positionValueY->setText(positionYBuffer);
    positionValueY->displayText(lcd);
    return;
  }

  float targetZ = cartesian.getTargetPosition(Axis::Z);

  if (cartesian.getPrevTargetPosition(Axis::Z) != targetZ)
  {
    LiquidLine *positionValueZ = &INFO_LINES[InfoLine::POS_LBL_Z_VAL];
    formatPosition(positionZBuffer, targetZ);

    positionValueZ->setText(positionZBuffer);
    positionValueZ->displayText(lcd);
    return;
  }

  tm prevTime = proc->getPrevTime();
  tm time = proc->getTime();

  if (prevTime.tm_hour != time.tm_hour || prevTime.tm_min != time.tm_min)
  {
    LiquidLine *processTimeValue = &INFO_LINES[InfoLine::PROC_TIME_LBL];
    formatProcessTime(time);

    processTimeValue->setText(processTimeBuffer);
    processTimeValue->displayText(lcd);
    return;
  }

  uint8_t prevProgress = proc->getPrevProgress();
  uint8_t progress = proc->getProgress();

  if (progress != prevProgress)
  {
    LiquidLine *progressBar = &INFO_LINES[InfoLine::PROG_BAR];
    LiquidLine *progressValue = &INFO_LINES[InfoLine::PROG_VAL];
    formatProgressBar(progress);
    formatProgressValue(progress);

    progressBar->setText(progressBarBuffer);
    progressBar->displayText(lcd);
    progressValue->setText(progressValueBuffer);
    progressValue->displayText(lcd);
  }
}

void displayAccelerationScreen(bool clear)
{
  Axis axis = cartesian.getCurrentAxis();

  if (clear)
  {
    LiquidLine *setAccelText = &SET_ACCEL_LINES[SetAccelerationLine::SET_ACCELERATION_LBL];

    switch (axis)
    {
    case Axis::X:
      rotary.setBoundaries(DEFAULT_MIN_ACCEL_X, DEFAULT_MAX_ACCEL_X);
      setAccelText->setText("Set X mm/s^2");
      break;

    case Axis::Y:
      rotary.setBoundaries(DEFAULT_MIN_ACCEL_Y, DEFAULT_MAX_ACCEL_Y);
      setAccelText->setText("Set Y mm/s^2");
      break;

    case Axis::Z:
      rotary.setBoundaries(DEFAULT_MIN_ACCEL_Z, DEFAULT_MAX_ACCEL_Z);
      setAccelText->setText("Set Z mm/s^2");
      break;
    }
  }

  LiquidLine *setAccelValue = &SET_ACCEL_LINES[SetAccelerationLine::SET_ACCELERATION_VAL];
  int accel = cartesian.getAcceleration(axis);

  clearBuffer(accelBuffer, ACCEL_VAL_SIZE);
  snprintf(accelBuffer, ACCEL_VAL_SIZE, "%-3i", accel);

  setAccelValue->setText(accelBuffer);

  rotary.setPosition(accel);

  if (clear)
  {
    display(ScreenID::SET_ACCEL);
  }
  else
  {
    setAccelValue->displayText(lcd);
  }
}

void displayMoveAxisScreen(bool clear)
{
  Axis axis = cartesian.getCurrentAxis();
  Unit unit = cartesian.getUnit();

  if (clear)
  {
    LiquidLine *moveAxisText = &MOVE_AXIS_LINES[MoveAxisLine::MOVE_AXIS_LBL];

    switch (axis)
    {
    case Axis::X:
      switch (unit)
      {
      case Unit::CENTIMETER:
        moveAxisText->setText("Move X 10mm");
        break;
      case Unit::MICROMETER:
        moveAxisText->setText("Move X 0.1mm");
        break;
      case Unit::MILLIMETER:
        moveAxisText->setText("Move X 1mm");
        break;
      }
      break;
    case Axis::Y:
      switch (unit)
      {
      case Unit::CENTIMETER:
        moveAxisText->setText("Move Y 10mm");
        break;
      case Unit::MICROMETER:
        moveAxisText->setText("Move Y 0.1mm");
        break;
      case Unit::MILLIMETER:
        moveAxisText->setText("Move Y 1mm");
        break;
      }
      break;

    case Axis::Z:
      switch (unit)
      {
      case Unit::CENTIMETER:
        moveAxisText->setText("Move Z 10mm");
        break;
      case Unit::MICROMETER:
        moveAxisText->setText("Move Z 0.1mm");
        break;
      case Unit::MILLIMETER:
        moveAxisText->setText("Move Z 1mm");
        break;
      }
      break;
    }
  }

  LiquidLine *moveAxisValue = &MOVE_AXIS_LINES[MoveAxisLine::MOVE_AXIS_VAL];
  float pos = cartesian.getTargetPosition(axis);

  clearBuffer(moveAxisBuffer, MOVE_AXIS_VAL_SIZE);
  snprintf(moveAxisBuffer, MOVE_AXIS_VAL_SIZE, "%-3.1f", pos / 1000);
  moveAxisValue->setText(moveAxisBuffer);

  rotary.setBoundaries(-400, 400);
  rotary.setPosition(pos);
  rotary.setInterval(100);

  if (clear)
  {
    display(ScreenID::MOVE_AXIS);
  }
  else
  {
    moveAxisValue->displayText(lcd);
  }
}

void displayLaserScreen(bool clear)
{
  LiquidLine *setPowerValue = &SET_POWER_LINES[SetPowerLine::SET_POWER_VAL];
  uint8_t maxPower = laser.getMaxPower();

  clearBuffer(powerBuffer, POWER_VAL_SIZE);
  snprintf(powerBuffer, POWER_VAL_SIZE, "%-3i", maxPower);
  setPowerValue->setText(powerBuffer);

  rotary.setBoundaries(LSR_MIN_POWER, LSR_MAX_POWER);
  rotary.setPosition(maxPower);

  if (clear)
  {
    display(ScreenID::SET_POWER);
  }
  else
  {
    setPowerValue->displayText(lcd);
  }
}

void displayStepsScreen(bool clear)
{
  Axis axis = cartesian.getCurrentAxis();

  if (clear)
  {
    LiquidLine *setStepsText = &SET_STEPS_LINES[SetStepsLine::SET_STEPS_LBL];

    switch (axis)
    {
    case Axis::X:
      rotary.setBoundaries(DEFAULT_MIN_STEPS_X, DEFAULT_MAX_STEPS_X);
      setStepsText->setText("Set X steps/mm");
      break;

    case Axis::Y:
      rotary.setBoundaries(DEFAULT_MIN_STEPS_Y, DEFAULT_MAX_STEPS_Y);
      setStepsText->setText("Set Y steps/mm");
      break;

    case Axis::Z:
      rotary.setBoundaries(DEFAULT_MIN_STEPS_Z, DEFAULT_MAX_STEPS_Z);
      setStepsText->setText("Set Z steps/mm");
      break;
    }
  }

  LiquidLine *setStepsValue = &SET_STEPS_LINES[SetStepsLine::SET_STEPS_VAL];
  long stepsPer = cartesian.getStepsPerMillimeter(axis);

  clearBuffer(stepsBuffer, STEPS_VAL_SIZE);
  snprintf(stepsBuffer, STEPS_VAL_SIZE, "%-3i", stepsPer);
  setStepsValue->setText(stepsBuffer);

  rotary.setPosition(stepsPer);

  if (clear)
  {
    display(ScreenID::SET_STEPS);
  }
  else
  {
    setStepsValue->displayText(lcd);
  }
}

void displayVelocityScreen(bool clear)
{
  Axis axis = cartesian.getCurrentAxis();

  if (clear)
  {
    LiquidLine *setVelocityText = &SET_VEL_LINES[SetVelocityLine::SET_VELOCITY_LBL];

    switch (axis)
    {
    case Axis::X:
      rotary.setBoundaries(DEFAULT_MIN_STEPS_X, DEFAULT_MAX_SPEED_X);
      setVelocityText->setText("Set X mm/s");
      break;

    case Axis::Y:
      rotary.setBoundaries(DEFAULT_MIN_STEPS_Y, DEFAULT_MAX_SPEED_Y);
      setVelocityText->setText("Set Y mm/s");
      break;

    case Axis::Z:
      rotary.setBoundaries(DEFAULT_MIN_STEPS_Z, DEFAULT_MAX_SPEED_Z);
      setVelocityText->setText("Set Z mm/s");
      break;
    }
  }

  LiquidLine *setVelocityValue = &SET_VEL_LINES[SetVelocityLine::SET_VELOCITY_VAL];
  int maxSpeed = cartesian.getFeedRate(axis);

  clearBuffer(velocityBuffer, VEL_VAL_SIZE);
  snprintf(velocityBuffer, VEL_VAL_SIZE, "%-3i", maxSpeed);
  setVelocityValue->setText(velocityBuffer);

  rotary.setPosition(maxSpeed);

  if (clear)
  {
    display(ScreenID::SET_VEL);
  }
  else
  {
    setVelocityValue->displayText(lcd);
  }
}

void autohome()
{
  commands.push(std::make_shared<AutohomeCommand>(cartesian, laser));
}

void setHomeOffsets()
{
  cartesian.stopSteppers();

  for (uint8_t i = 0; i < AXES; i++)
  {
    Axis axis = static_cast<Axis>(i);
    float u = cartesian.toUnit(axis, Unit::MILLIMETER);

    cartesian.setHomeOffset(axis, u);
  }
}

void storeSettings()
{
}

void terminateProcess(bool clear, Icon icon)
{
  MAIN_MENU_SCREEN.hide(MainLine::RESUME_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainLine::PAUSE_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainLine::STOP_PROC_OP);

  MAIN_MENU_SCREEN.unhide(MainLine::PREP_MENU_RTN);
  MAIN_MENU_SCREEN.unhide(MainLine::START_FROM_TF_OP);

  proc->stop();

  if (clear)
  {
    displayInfoScreen();
  }

  displayIcon(icon);
}

void completeProcess()
{
  terminateProcess(false, Icon::CHECK);
}

void pauseProcess()
{
  MAIN_MENU_SCREEN.unhide(MainLine::RESUME_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainLine::PAUSE_PROC_OP);

  proc->pause();

  displayInfoScreen();
  displayIcon(Icon::PAUSE);
}

void resumeProcess()
{
  MAIN_MENU_SCREEN.unhide(MainLine::PAUSE_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainLine::RESUME_PROC_OP);

  proc->setup();

  displayInfoScreen();
  displayIcon(Icon::PLAY);
}

void stopProcess(bool clear)
{
  terminateProcess(clear, Icon::STOP);
}

void aboutScreenClicked()
{
  displayMenu(ScreenID::MAIN, true);
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
    displayAccelerationScreen(true);
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Y);
    displayAccelerationScreen(true);
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Z);
    displayAccelerationScreen(true);
    break;
  default:
    break;
  }
}

void cardScreenClicked()
{
  uint8_t index = CARD_MENU_SCREEN.getCurrentLineIndex();

  if (index > 0)
  {
    const char *filename = paths[index - 1].c_str();

    clearBuffer(pathBuffer, FILE_PATH_SIZE);
    snprintf(pathBuffer, FILE_PATH_SIZE, "/%s", filename);

    processes.push(std::make_shared<TFProcess>(SD, rtc, cartesian, laser, pathBuffer, filename));

    MAIN_MENU_SCREEN.unhide(MainLine::PAUSE_PROC_OP);
    MAIN_MENU_SCREEN.unhide(MainLine::STOP_PROC_OP);
    MAIN_MENU_SCREEN.hide(MainLine::PREP_MENU_RTN);
    MAIN_MENU_SCREEN.hide(MainLine::START_FROM_TF_OP);
    MAIN_MENU_SCREEN.hide(MainLine::RESUME_PROC_OP);

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
    displayLaserScreen(true);
    break;
  case 3:
    storeSettings();
    displayInfoScreen();
    break;
  default:
    break;
  }
}

void infoScreenClicked()
{
  displayMenu(ScreenID::MAIN, false);
}

void mainScreenClicked()
{
  switch (MAIN_MENU_SCREEN.getCurrentLineIndex())
  {
  case MainLine::INFO_MENU_RTN:
    displayInfoScreen();
    break;

  case MainLine::PREP_MENU_RTN:
    displayMenu(ScreenID::PREP, false);
    break;

  case MainLine::CTRL_MENU_RTN:
    displayMenu(ScreenID::CTRL, false);
    break;

  case MainLine::NO_TF_CARD_OP:
    displayInfoScreen();
    break;

  case MainLine::START_FROM_TF_OP:
    displayMenu(ScreenID::CARD, false);
    break;

  case MainLine::PAUSE_PROC_OP:
    pauseProcess();
    break;

  case MainLine::RESUME_PROC_OP:
    resumeProcess();
    break;

  case MainLine::STOP_PROC_OP:
    stopProcess(true);
    break;

  case MainLine::ABOUT_CNC_OP:
    displayAboutScreen();
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
  rotary.setInterval(1);
  displayMenu(ScreenID::ACCEL, true);
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
    displayMoveAxisScreen(true);
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::X);
    cartesian.setUnit(Unit::MILLIMETER);
    displayMoveAxisScreen(true);
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::X);
    cartesian.setUnit(Unit::MICROMETER);
    displayMoveAxisScreen(true);
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
    displayMoveAxisScreen(true);
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Y);
    cartesian.setUnit(Unit::MILLIMETER);
    displayMoveAxisScreen(true);
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Y);
    cartesian.setUnit(Unit::MICROMETER);
    displayMoveAxisScreen(true);
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
    displayMoveAxisScreen(true);
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Z);
    cartesian.setUnit(Unit::MILLIMETER);
    displayMoveAxisScreen(true);
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Z);
    cartesian.setUnit(Unit::MICROMETER);
    displayMoveAxisScreen(true);
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
  rotary.setInterval(1);
  displayMenu(ScreenID::ACCEL, true);
}

void setPowerScreenClicked()
{
  rotary.setInterval(1);
  displayMenu(ScreenID::CTRL, true);
}

void setStepsScreenClicked()
{
  rotary.setInterval(1);
  displayMenu(ScreenID::STEPS, true);
}

void setVelScreenClicked()
{
  rotary.setInterval(1);
  displayMenu(ScreenID::VEL, true);
}

void stepsScreenClicked()
{
  switch (STEPS_MENU_SCREEN.getCurrentLineIndex())
  {
  case 0:
    displayMenu(ScreenID::MOTION, true);
    break;
  case 1:
    cartesian.setCurrentAxis(Axis::X);
    displayStepsScreen(true);
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Y);
    displayStepsScreen(true);
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Z);
    displayStepsScreen(true);
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
    displayVelocityScreen(true);
    break;
  case 2:
    cartesian.setCurrentAxis(Axis::Y);
    displayVelocityScreen(true);
    break;
  case 3:
    cartesian.setCurrentAxis(Axis::Z);
    displayVelocityScreen(true);
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

void moveAxis(Rotation direction)
{
  float u = cartesian.getUnit() == Unit::MICROMETER ? 100 : 1; // move 0.1mm if unit micron

  if (direction == Rotation::COUNTERCLOCKWISE)
  {
    u = -u;
  }

  switch (cartesian.getCurrentAxis())
  {
  case Axis::X:
    commands.push(std::make_shared<LinearMoveCommand>(cartesian, laser, u, 0, 0, FEED_RATE_MOVE_AXIS, 0));
    break;
  case Axis::Y:
    commands.push(std::make_shared<LinearMoveCommand>(cartesian, laser, u, 0, 0, FEED_RATE_MOVE_AXIS, 0));
    break;
  case Axis::Z:
    commands.push(std::make_shared<LinearMoveCommand>(cartesian, laser, u, 0, 0, FEED_RATE_MOVE_AXIS, 0));
  }

  displayMoveAxisScreen(false);
}

void setAcceleration()
{
  Axis axis = cartesian.getCurrentAxis();
  long pos = rotary.getPosition();

  cartesian.setAcceleration(axis, Unit::MILLIMETER, pos);

  displayAccelerationScreen(false);
}

void setPower(Rotation rotation)
{
  Axis axis = cartesian.getCurrentAxis();
  long pos = rotary.getPosition();

  laser.setMaxPower(pos);
  displayLaserScreen(false);
}

void setSteps(Rotation rotation)
{
  Axis axis = cartesian.getCurrentAxis();
  long pos = rotary.getPosition();

  cartesian.setStepsPerMillimeter(axis, pos);
  displayStepsScreen(false);
}

void setVelocity(Rotation rotation)
{
  Axis axis = cartesian.getCurrentAxis();
  long pos = rotary.getPosition();

  cartesian.setMaxSpeed(axis, Unit::MILLIMETER, pos);
  displayVelocityScreen(false);
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

void setupAboutScreen()
{
  for (uint8_t i = 0; i < ABOUT_SIZE; i++)
  {
    ABOUT_SCREEN.append(i, ABOUT_LINES[i]);
  }
}

void setupAccelerationScreen()
{
  for (uint8_t i = 0; i < ACCEL_SIZE; i++)
  {
    ACCEL_MENU_SCREEN.append(i, ACCEl_LINES[i]);
  }
}

void setupCardScreen()
{
  loadFromTF();

  for (uint8_t i = 0; i < nFiles + 1; i++)
  {
    if (i > 0)
    {
      CARD_LINES[i] = LiquidLine(MENU_COL, FIRST_ROW, paths[i - 1].c_str());
    }
    else
    {
      CARD_LINES[i] = LiquidLine(MENU_COL, FIRST_ROW, "Main");
    }

    CARD_MENU_SCREEN.append(i, CARD_LINES[i]);
  }
}

void setupCtrlScreen()
{
  for (uint8_t i = 0; i < CTRL_SIZE; i++)
  {
    CTRL_MENU_SCREEN.append(i, CTRL_LINES[i]);
  }
}

void setupInfoScreen()
{
  for (uint8_t i = 0; i < INFO_SIZE; i++)
  {
    INFO_SCREEN.append(i, INFO_LINES[i]);
  }
}

void setupMainScreen()
{
  for (uint8_t i = 0; i < MAIN_SIZE; i++)
  {
    MAIN_MENU_SCREEN.append(i, MAIN_LINES[i]);
  }

  MAIN_MENU_SCREEN.hide(MainLine::NO_TF_CARD_OP);
  MAIN_MENU_SCREEN.hide(MainLine::PAUSE_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainLine::RESUME_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainLine::STOP_PROC_OP);
}

void setupMoveAxesScreen()
{
  for (uint8_t i = 0; i < MOVE_AXES_SIZE; i++)
  {
    MOVE_AXES_MENU_SCREEN.append(i, MOVE_AXES_LINES[i]);
  }
}

void setupMoveAxisScreen()
{
  for (uint8_t i = 0; i < MOVE_AXIS_SIZE; i++)
  {
    MOVE_AXIS_SCREEN.append(i, MOVE_AXIS_LINES[i]);
  }
}

void setupMoveXScreen()
{
  for (uint8_t i = 0; i < MOVE_X_SIZE; i++)
  {
    MOVE_X_MENU_SCREEN.append(i, MOVE_X_LINES[i]);
  }
}

void setupMoveYScreen()
{
  for (uint8_t i = 0; i < MOVE_Y_SIZE; i++)
  {
    MOVE_Y_MENU_SCREEN.append(i, MOVE_Y_LINES[i]);
  }
}

void setupMoveZScreen()
{
  for (uint8_t i = 0; i < MOVE_Z_SIZE; i++)
  {
    MOVE_Z_MENU_SCREEN.append(i, MOVE_Z_LINES[i]);
  }
}

void setupMotionScreen()
{
  for (uint8_t i = 0; i < MOTION_SIZE; i++)
  {
    MOTION_MENU_SCREEN.append(i, MOTION_LINES[i]);
  }
}

void setupPrepScreen()
{
  for (uint8_t i = 0; i < PREP_SIZE; i++)
  {
    PREP_MENU_SCREEN.append(i, PREP_LINES[i]);
  }
}

void setupSetAccelerationScreen()
{
  for (uint8_t i = 0; i < SET_ACCEL_SIZE; i++)
  {
    SET_ACCEL_SCREEN.append(i, SET_ACCEL_LINES[i]);
  }
}

void setupSetPowerScreen()
{
  for (uint8_t i = 0; i < SET_POWER_SIZE; i++)
  {
    SET_POWER_SCREEN.append(i, SET_POWER_LINES[i]);
  }
}

void setupSetStepsScreen()
{
  for (uint8_t i = 0; i < SET_STEPS_SIZE; i++)
  {
    SET_STEPS_SCREEN.append(i, SET_STEPS_LINES[i]);
  }
}

void setupSetVelocityScreen()
{
  for (uint8_t i = 0; i < SET_VEL_SIZE; i++)
  {
    SET_VEL_SCREEN.append(i, SET_VEL_LINES[i]);
  }
}

void setupStepsScreen()
{
  for (uint8_t i = 0; i < STEPS_SIZE; i++)
  {
    STEPS_MENU_SCREEN.append(i, STEPS_LINES[i]);
  }
}

void setupVelocityScreen()
{
  for (uint8_t i = 0; i < VEL_SIZE; i++)
  {
    VEL_MENU_SCREEN.append(i, VEL_LINES[i]);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(SDA, PULLUP);
  pinMode(SCL, PULLUP);

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

  lcd.createChar(ICON_CHAR, ARROW_CHAR);
  lcd.createChar(LCD_GAUGE_FILL_1, GAUGE_FILL_1);
  lcd.createChar(LCD_GAUGE_FILL_2, GAUGE_FILL_2);
  lcd.createChar(LCD_GAUGE_FILL_3, GAUGE_FILL_3);
  lcd.createChar(LCD_GAUGE_FILL_4, GAUGE_FILL_4);
  lcd.createChar(LCD_GAUGE_LEFT, GAUGE_LEFT);
  lcd.createChar(LCD_GAUGE_RIGHT, GAUGE_RIGHT);
  lcd.createChar(LCD_GAUGE_EMPTY, GAUGE_EMPTY);

  laser.setMaxPower(LSR_MAX_POWER);

  cartesian.setFeedRate(Axis::X, DEFAULT_FEED_RATE);
  cartesian.setFeedRate(Axis::Y, DEFAULT_FEED_RATE);
  cartesian.setFeedRate(Axis::Z, DEFAULT_FEED_RATE);

  cartesian.setAcceleration(Axis::X, Unit::MILLIMETER, DEFAULT_MIN_ACCEL_X);
  cartesian.setAcceleration(Axis::Y, Unit::MILLIMETER, DEFAULT_MIN_ACCEL_Y);
  cartesian.setAcceleration(Axis::Z, Unit::MILLIMETER, DEFAULT_MIN_ACCEL_Z);

  cartesian.setDimension(Axis::X, DIMENSIONS_X);
  cartesian.setDimension(Axis::Y, DIMENSIONS_Y);
  cartesian.setDimension(Axis::Z, DIMENSIONS_Z);

  cartesian.setLimitSwitch(Axis::X, switchX);
  cartesian.setLimitSwitch(Axis::Y, switchY);
  cartesian.setLimitSwitch(Axis::Z, switchZ);

  cartesian.setMaxSpeed(Axis::X, Unit::MILLIMETER, DEFAULT_MAX_SPEED_X);
  cartesian.setMaxSpeed(Axis::Y, Unit::MILLIMETER, DEFAULT_MAX_SPEED_Y);
  cartesian.setMaxSpeed(Axis::Z, Unit::MILLIMETER, DEFAULT_MAX_SPEED_Z);

  cartesian.setMinStepsPerMillimeter(Axis::X, DEFAULT_MIN_STEPS_X);
  cartesian.setMinStepsPerMillimeter(Axis::Y, DEFAULT_MIN_STEPS_Y);
  cartesian.setMinStepsPerMillimeter(Axis::Z, DEFAULT_MIN_STEPS_Z);

  cartesian.setStepperMotor(Axis::X, stepperX);
  cartesian.setStepperMotor(Axis::Y, stepperY);
  cartesian.setStepperMotor(Axis::Z, stepperZ);

  cartesian.setStepsPerMillimeter(Axis::X, DEFAULT_STEPS_X);
  cartesian.setStepsPerMillimeter(Axis::Y, DEFAULT_STEPS_Y);
  cartesian.setStepsPerMillimeter(Axis::Z, DEFAULT_STEPS_Z);

  rotary.setOnClicked(clickCallback);
  rotary.setOnRotation(rotateCallback);

  rotary.setClickDebounceTime(EN_CLICK_DEBOUNCE);
  rotary.setRotationDebounceTime(EN_ROT_DEBOUNCE);

  switchX.setDebounceTime(DEBOUNCE_TIME);
  switchY.setDebounceTime(DEBOUNCE_TIME);
  switchZ.setDebounceTime(DEBOUNCE_TIME);

  root = SD.open("/");

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
      displayIcon(Icon::PLAY);
      proc->setup();
    }
  }

  if (proc)
  {
    // process is not available & there's no commands left
    if (!proc->continues() && commands.empty() && !cmd)
    {
      completeProcess();
      proc = nullptr;
      delay(PROCESS_EXPIRED);
      displayInfoScreen();
      return;
    }

    if (proc->isStopped())
    {
      stopProcess(false);
      proc = nullptr;
      delay(PROCESS_EXPIRED);
      displayInfoScreen();
      return;
    }

    updateInfoScreen();

    // execute commands if not paused & stopped
    if (!proc->isPaused() && !proc->isStopped())
    {
      proc->nextCommand(commands);

      if (!commands.empty() && !cmd)
      {
        cmd = commands.front();
        commands.pop();

        if (cmd)
        {
          cmd->setup();
        }
      }

      if (cmd)
      {
        if (!cmd->continues())
        {
          cmd->stop();
          cmd = nullptr;
          return;
        }

        cmd->execute();
      }
    }
  }
}
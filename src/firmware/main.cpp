#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include <Command.h>
#include <LimitSwitch.h>
#include <LiquidScreen.h>
#include <Process.h>

#define CMD_QUEUE_SIZE 10
#define MAX_FILES_SIZE 11

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

#define PROC_ICON_VAL_COL 0
#define PROC_NAME_LBL_COL 2
#define PROC_TIME_LBL_COL 15

#define PROG_BAR_COL 0
#define PROG_VAL_COL 15

#define POS_LBL_X_COL 0
#define POS_LBL_Y_COL 6
#define POS_LBL_Z_COL 12

#define POS_VAL_X_COL 1
#define POS_VAL_Y_COL 7
#define POS_VAL_Z_COL 13

#define LSR_PIN 32 // TODO: use GPIO 13

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

Cartesian cartesian;
RTC rtc;

LiquidScreen ABOUT_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen INFO_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);

LiquidScreen MOVE_AXIS_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_ACCEL_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_POWER_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_STEPS_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen SET_VEL_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MAIN_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu PREP_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CTRL_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CARD_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOVE_AXES_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_X_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Y_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Z_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu ACCEL_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu VEL_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOTION_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu STEPS_MENU_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

char INTERNAL_FILE_EXT = '~';
const char *EMPTY_ROW = "                    ";
const char *SYSTEM_VOLUME = "System Volume Information";
const char *GCODE_FILE_EXT = ".gcode";

const uint16_t DEFAULT_FEED_RATE = 3000;
const uint8_t DEFAULT_MAX_POWER = 100;

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

byte ARROW_CHAR[8] = {B00000, B00100, B00110, B11111, B11111, B00110, B00100, B00000};

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
    LiquidLine(PROC_NAME_LBL_COL, THIRD_ROW, "Ready!"),
    LiquidLine(PROC_TIME_LBL_COL, THIRD_ROW, "00:00"),
    LiquidLine(PROG_BAR_COL, FOURTH_ROW, ""),
    LiquidLine(PROG_VAL_COL, FOURTH_ROW, "0%")};

LiquidLine MAIN_LINES[MAIN_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Info"),
    LiquidLine(MENU_COL, FIRST_ROW, "Prepare"),
    LiquidLine(MENU_COL, FIRST_ROW, "Control"),
    LiquidLine(MENU_COL, FIRST_ROW, "No TF Card"),
    LiquidLine(MENU_COL, FIRST_ROW, "Start from TF"),
    LiquidLine(MENU_COL, FIRST_ROW, "Pause"),
    LiquidLine(MENU_COL, FIRST_ROW, "Resume"),
    LiquidLine(MENU_COL, FIRST_ROW, "Stop"),
    LiquidLine(MENU_COL, FIRST_ROW, "About CNC")};

LiquidLine MOVE_AXES_LINES[MOVE_AXES_SIZE] = {
    LiquidLine(MENU_COL, FIRST_ROW, "Prepare"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move X"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Y"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move Z")};

LiquidLine MOVE_AXIS_LINES[MOVE_AXIS_SIZE] = {
    LiquidLine(SIXTH_COL, SECOND_ROW, "Move X 1mm"),
    LiquidLine(SIXTH_COL, THIRD_ROW, "0")};

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
    LiquidLine(MENU_COL, FIRST_ROW, "Main"),
    LiquidLine(MENU_COL, FIRST_ROW, "Move axes"),
    LiquidLine(MENU_COL, FIRST_ROW, "Auto home"),
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
    LiquidLine(FOURTH_COL, SECOND_ROW, "Set X Velocity"),
    LiquidLine(FOURTH_COL, THIRD_ROW, "0")};

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

CommandQueue commands;
ProcessQueue processes;
CommandPtr cmd;
ProcessPtr proc;

const char *paths[MAX_FILES_SIZE];
uint8_t nFiles;
File root;

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
      CARD_LINES[i] = LiquidLine(MENU_COL, FIRST_ROW, paths[i - 1]);
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

  MAIN_MENU_SCREEN.hide(MainOption::NO_TF_CARD_OP);
  MAIN_MENU_SCREEN.hide(MainOption::PAUSE_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainOption::RESUME_PROC_OP);
  MAIN_MENU_SCREEN.hide(MainOption::STOP_PROC_OP);
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

  laser.setMaxPower(DEFAULT_MAX_POWER);
  cartesian.setFeedRate(Axis::X, DEFAULT_FEED_RATE);
  cartesian.setFeedRate(Axis::Y, DEFAULT_FEED_RATE);
  cartesian.setFeedRate(Axis::Z, DEFAULT_FEED_RATE);
  cartesian.disableSteppers();

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

  CARD_MENU_SCREEN.display();

  processes.push(std::make_shared<TFProcess>(SD, rtc, cartesian, laser, "/star.gcode", "star.gcode"));
}

void loop()
{
  if (!processes.empty() && !proc)
  {
    proc = processes.front();
    processes.pop();

    if (proc)
    {
      proc->setup();
    }
  }

  if (proc)
  {
    if (!proc->continues())
    {
      proc->stop();
      // pops next process
      // TODO: reset
      // disable steppers
      // set feed rate to default
      // set power to 0
      proc = nullptr;
      return;
    }

    proc->nextCommand(commands);

    if (!commands.empty() && !cmd)
    {
      cmd = commands.front();
      commands.pop();

      if (cmd)
      {
        cmd->setup();
        // TODO: remove
        delay(200);
      }
    }

    if (cmd)
    {
      if (!cmd->continues())
      {
        cmd->stop();
        // pops next command
        cmd = nullptr;
        return;
      }

      cmd->execute();
    }
  }
}
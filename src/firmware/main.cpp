#include <Cartesian.h>
#include <Command.h>
#include <Process.h>
#include <LimitSwitch.h>
#include <LiquidScreen.h>
#include <Rotary.h>
#include <StepperMotor.h>

#include <LiquidCrystal_I2C.h>
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

#define LCD_ARROW_CHAR 0
#define LCD_GAUGE_FILL_1 1
#define LCD_GAUGE_FILL_2 2
#define LCD_GAUGE_FILL_3 3
#define LCD_GAUGE_FILL_4 4
#define LCD_GAUGE_LEFT 5
#define LCD_GAUGE_RIGHT 6
#define LCD_GAUGE_EMPTY 7

#define POS_LBL_X_COL 0
#define POS_LBL_Y_COL 6
#define POS_LBL_Z_COL 12

#define POS_VAL_X_COL 1
#define POS_VAL_Y_COL 7
#define POS_VAL_Z_COL 13

#define PROC_NAME_LBL_COL 0
#define PROC_TIME_LBL_COL 15

#define PROG_BAR_COL 0
#define PROG_VAL_COL 15

#define LCD_ARROW_COL 2
#define LCD_ZERO_COL 0
#define LCD_ZERO_ROW 0

#define PROG_VAL_SIZE 4
#define POS_VAL_SIZE 4
#define LSR_MAX_POWER 100

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
  ACCELERATION,
  CARD,
  CTRL,
  INFO,
  LASER,
  MAIN,
  MOTION,
  MOVE_AXES,
  MOVE_AXIS,
  MOVE_X,
  MOVE_Y,
  MOVE_Z,
  PREP,
  STEPS,
  VELOCITY
};

enum MainOption
{
  INFO_MENU,
  PREP_MENU,
  CTRL_MENU,
  NO_TF_CARD,
  START_FROM_TF,
  PAUSE_PROC,
  RESUME_PROC,
  STOP_PROC,
  ABOUT_CNC
};

byte ARROW_CHAR[8] = {B00000, B00100, B00110, B11111, B11111, B00110, B00100, B00000};
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

const int CHAR_PIXEL_WIDTH = 5;
const int EMPTY_SPACE = 255;
const int GAUGE_SIZE = 14;

String EMPTY_ROW("                    ");
String SYSTEM_VOLUME("System Volume Information");

char INTERNAL_FILE_EXT = '~';
String GCODE_FILE_EXT(".gcode");

SPIClass spi(VSPI);

Cartesian cartesian;
RTC rtc;

Laser laser(LSR_PIN);
LCD lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

ShiftRegisterMotorInterface SHIFT_REGISTER_X1(spi, CS_RS1_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_X2(spi, CS_RS2_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Y(spi, CS_RS3_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Z(spi, CS_RS4_PIN, MSBFIRST);

BilateralMotorInterface BILATERAL_X(SHIFT_REGISTER_X1, SHIFT_REGISTER_X2);

StepperMotorPtr stepperX = std::make_shared<StepperMotor>(BILATERAL_X);
StepperMotorPtr stepperY = std::make_shared<StepperMotor>(SHIFT_REGISTER_Y);
StepperMotorPtr stepperZ = std::make_shared<StepperMotor>(SHIFT_REGISTER_Z);

LimitSwitchPtr switchX = std::make_shared<LimitSwitch>(SW_X_PIN);
LimitSwitchPtr switchY = std::make_shared<LimitSwitch>(SW_Y_PIN);
LimitSwitchPtr switchZ = std::make_shared<LimitSwitch>(SW_Z_PIN);

LiquidScreen INFO_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen ABOUT_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen MOVE_AXIS_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MAIN_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu PREP_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CTRL_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CARD_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu LASER_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOVE_AXES_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_X_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Y_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Z_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOTION_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu VELOCITY_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu STEPS_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu ACCEL_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidLinePtr positionValueX;
LiquidLinePtr positionValueY;
LiquidLinePtr positionValueZ;

LiquidLinePtr processNameValue;
LiquidLinePtr processTimeValue;
LiquidLinePtr progressBar;
LiquidLinePtr progressValue;

byte gauge_left_dynamic[8];
byte gauge_right_dynamic[8];

String filePaths[MAX_FILES_SIZE];
ScreenID currentScreen;
File root;

queue<std::shared_ptr<Command>> commands;
queue<std::shared_ptr<Process>> processes;

std::shared_ptr<Command> cmd;
std::shared_ptr<Process> proc;

LiquidScreen *getScreen(ScreenID screenIndex)
{
  switch (screenIndex)
  {
  case ABOUT:
    return &ABOUT_SCREEN;
  case ACCELERATION:
    return &ACCEL_SCREEN;
  case CARD:
    return &CARD_SCREEN;
  case CTRL:
    return &CTRL_SCREEN;
  case INFO:
    return &INFO_SCREEN;
  case LASER:
    return &LASER_SCREEN;
  case MAIN:
    return &MAIN_SCREEN;
  case MOTION:
    return &MOTION_SCREEN;
  case MOVE_AXES:
    return &MOVE_AXES_SCREEN;
  case MOVE_AXIS:
    return &MOVE_AXIS_SCREEN;
  case MOVE_X:
    return &MOVE_X_SCREEN;
  case MOVE_Y:
    return &MOVE_Y_SCREEN;
  case MOVE_Z:
    return &MOVE_Z_SCREEN;
  case PREP:
    return &PREP_SCREEN;
  case STEPS:
    return &STEPS_SCREEN;
  case VELOCITY:
    return &VELOCITY_SCREEN;
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
    rotary.setPosition(forcePosition ? screen->getCurrentLineIndex() : 0);

    currentScreen = screenIndex;
    screen->display();
  }
}

void displayInfoScreen()
{
  unsigned long start = millis();
  if (proc)
  {
    processNameValue->setText(proc->getName());
    processTimeValue->setText(proc->getTime());

    positionValueX->setText(formatPosition(Axis::X));
    positionValueY->setText(formatPosition(Axis::Y));
    positionValueZ->setText(formatPosition(Axis::Z));

    uint8_t progress = proc->getProgress();

    updateProgressBar(progress);
    progressValue->setText(formatProgress(progress));
  }
  else
  {
    positionValueX->setText("0");
    positionValueY->setText("0");
    positionValueZ->setText("0");

    processNameValue->setText("Ready!");
    processTimeValue->setText("00:00");

    updateProgressBar(0);
    progressValue->setText("0%");
  }

  display(ScreenID::INFO, true);
}

void setMoveAxisScreen(Axis axis, uint8_t unit)
{
  display(ScreenID::MOVE_AXIS, false);
}

void autohome()
{
  commands.push(std::make_shared<AutohomeCommand>(cartesian, laser));
  displayInfoScreen();
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
  displayInfoScreen();
}

void aboutScreenClicked()
{
  displayInfoScreen();
}

void storeSettings()
{
  displayInfoScreen();
}

void accelerationScreenClicked()
{
  switch (ACCEL_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::MOTION, true);
    break;
  case 1:
    break;
  default:
    break;
  }
}

void cardScreenClicked()
{
  uint8_t index = CARD_SCREEN.getCurrentLineIndex();

  if (index > 0)
  {
    processes.push(std::make_shared<Process>(SD, rtc, "/", filePaths[index - 1]));
    cartesian.enableSteppers();

    MAIN_SCREEN.unhide(MainOption::PAUSE_PROC);
    MAIN_SCREEN.unhide(MainOption::STOP_PROC);
    MAIN_SCREEN.hide(MainOption::START_FROM_TF);
    MAIN_SCREEN.hide(MainOption::RESUME_PROC);

    displayInfoScreen();
  }
  else
  {
    display(ScreenID::MAIN, true);
  }
}

void ctrlScreenClicked()
{
  switch (CTRL_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::MAIN, true);
    break;
  case 1:
    display(ScreenID::MOTION, false);
    break;
  case 2:
    display(ScreenID::LASER, false);
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
  switch (MAIN_SCREEN.getCurrentLineIndex())
  {
  case MainOption::INFO_MENU:
    displayInfoScreen();
    break;

  case MainOption::PREP_MENU:
    display(ScreenID::PREP, false);
    break;

  case MainOption::CTRL_MENU:
    display(ScreenID::CTRL, false);
    break;

  case MainOption::NO_TF_CARD:
    displayInfoScreen();
    break;

  case MainOption::START_FROM_TF:
    display(ScreenID::CARD, false);
    break;

  case MainOption::PAUSE_PROC:
    MAIN_SCREEN.unhide(MainOption::RESUME_PROC);
    MAIN_SCREEN.hide(MainOption::PAUSE_PROC);
    displayInfoScreen();
    proc->pause();
    break;

  case MainOption::RESUME_PROC:
    MAIN_SCREEN.unhide(MainOption::PAUSE_PROC);
    MAIN_SCREEN.hide(MainOption::RESUME_PROC);
    displayInfoScreen();
    proc->resume();
    break;

  case MainOption::STOP_PROC:
    MAIN_SCREEN.unhide(MainOption::START_FROM_TF);
    MAIN_SCREEN.hide(MainOption::RESUME_PROC);
    MAIN_SCREEN.hide(MainOption::PAUSE_PROC);
    MAIN_SCREEN.hide(MainOption::STOP_PROC);
    displayInfoScreen();
    proc->stop();
    break;

  case MainOption::ABOUT_CNC:
    display(ScreenID::ABOUT, false);
    break;

  default:
    break;
  }
}

void motionScreenClicked()
{
  switch (MOTION_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::CTRL, true);
    break;
  case 1:
    display(ScreenID::ACCELERATION, false);
    break;
  case 2:
    display(ScreenID::VELOCITY, false);
    break;
  case 3:
    display(ScreenID::STEPS, false);
    break;
  default:
    break;
  }
}

void moveAxesScreenClicked()
{
  switch (MOVE_AXES_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(PREP, true);
    break;
  case 1:
    display(ScreenID::MOVE_X, false);
    break;
  case 2:
    display(ScreenID::MOVE_Y, false);
    break;
  case 3:
    display(ScreenID::MOVE_Z, false);
    break;
  default:
    break;
  }
}

void moveAxisScreenClicked()
{
}

void moveXScreenClicked()
{
}

void moveYScreenClicked()
{
}

void moveZScreenClicked()
{
}

void prepScreenClicked()
{
  switch (PREP_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::MAIN, true);
    break;
  case 1:
    display(MOVE_AXES, false);
    break;
  case 2:
    autohome();
    break;
  case 3:
    setHomeOffsets();
    break;
  case 4:
    cartesian.disableSteppers();
    displayInfoScreen();
    break;
  default:
    break;
  }
}

void stepsScreenClicked()
{
  switch (STEPS_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(MOTION, true);
    break;
  case 1:
    break;
  default:
    break;
  }
}

void velocityScreenClicked()
{
  switch (VELOCITY_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(MOTION, true);
    break;
  case 1:
    break;
  default:
    break;
  }
}

void clickCallback()
{
  switch (currentScreen)
  {
  case ABOUT:
    aboutScreenClicked();
    break;
  case ACCELERATION:
    accelerationScreenClicked();
    break;
  case CARD:
    cardScreenClicked();
    break;
  case CTRL:
    ctrlScreenClicked();
    break;
  case INFO:
    infoScreenClicked();
    break;
  case MAIN:
    mainScreenClicked();
    break;
  case MOTION:
    motionScreenClicked();
    break;
  case MOVE_AXIS:
    moveAxisScreenClicked();
    break;
  case MOVE_AXES:
    moveAxesScreenClicked();
    break;
  case MOVE_X:
    moveXScreenClicked();
    break;
  case MOVE_Y:
    moveYScreenClicked();
    break;
  case MOVE_Z:
    moveZScreenClicked();
    break;
  case PREP:
    prepScreenClicked();
    break;
  case STEPS:
    stepsScreenClicked();
    break;
  case VELOCITY:
    velocityScreenClicked();
    break;
  default:
    break;
  }
}

void rotateCallback(Rotation direction)
{
  LiquidScreen *screen = getScreen(currentScreen);

  if (screen)
  {
    if (currentScreen == ScreenID::INFO)
    {
      return;
    }

    if (currentScreen == ScreenID::MOVE_AXIS)
    {
      return;
    }

    if (direction == Rotation::CLOCKWISE)
    {
      screen->nextLine();
    }
    else
    {
      screen->previousLine();
    }

    screen->display(false);
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

LiquidLinePtr createLine(LiquidScreen &screen, uint8_t lineIndex, uint8_t row, uint8_t col, String text, size_t textLength)
{
  LiquidLinePtr ptr = std::make_shared<LiquidLine>(row, col, pad_right(text.c_str(), textLength).c_str());

  screen.append(lineIndex, ptr);
  return ptr;
}

void setupAccelScreen()
{
  createLine(ACCEL_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion", 7);
  createLine(ACCEL_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "Accel", 7);
  createLine(ACCEL_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Travel", 7);
  createLine(ACCEL_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax X", 7);
  createLine(ACCEL_SCREEN, 4, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax Y", 7);
  createLine(ACCEL_SCREEN, 5, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax Z", 7);
}

void setupAboutScreen()
{
  createLine(ABOUT_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Main", 4);
}

void setupCardScreen()
{
  createLine(CARD_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Main", 4);

  uint8_t n = getFiles(root, filePaths, MAX_FILES_SIZE);

  for (uint8_t i = 0; i < n; i++)
  {
    createLine(CARD_SCREEN, i + 1, LCD_ARROW_COL, LCD_ZERO_ROW, filePaths[i].c_str(), 18);
  }
}

void setupCtrlScreen()
{
  createLine(CTRL_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Main", 15);
  createLine(CTRL_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion", 15);
  createLine(CTRL_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Laser", 15);
  createLine(CTRL_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Store settings", 15);
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

  processNameValue = createLine(INFO_SCREEN, 7, PROC_NAME_LBL_COL, 2, "Ready!", 0);
  processTimeValue = createLine(INFO_SCREEN, 8, PROC_TIME_LBL_COL, 2, "00:00", 0);

  progressBar = createLine(INFO_SCREEN, 9, PROG_BAR_COL, 3, "", 0);
  progressValue = createLine(INFO_SCREEN, 10, PROG_VAL_COL, 3, "0%", 0);
}

void setupMainScreen()
{
  createLine(MAIN_SCREEN, MainOption::INFO_MENU, LCD_ARROW_COL, LCD_ZERO_ROW, "Info", 14);
  createLine(MAIN_SCREEN, MainOption::PREP_MENU, LCD_ARROW_COL, LCD_ZERO_ROW, "Prepare", 14);
  createLine(MAIN_SCREEN, MainOption::CTRL_MENU, LCD_ARROW_COL, LCD_ZERO_ROW, "Control", 14);
  createLine(MAIN_SCREEN, MainOption::NO_TF_CARD, LCD_ARROW_COL, LCD_ZERO_ROW, "No TF Card", 14);
  createLine(MAIN_SCREEN, MainOption::START_FROM_TF, LCD_ARROW_COL, LCD_ZERO_ROW, "Start from TF", 14);
  createLine(MAIN_SCREEN, MainOption::PAUSE_PROC, LCD_ARROW_COL, LCD_ZERO_ROW, "Pause", 14);
  createLine(MAIN_SCREEN, MainOption::RESUME_PROC, LCD_ARROW_COL, LCD_ZERO_ROW, "Resume", 14);
  createLine(MAIN_SCREEN, MainOption::STOP_PROC, LCD_ARROW_COL, LCD_ZERO_ROW, "Stop", 14);
  createLine(MAIN_SCREEN, MainOption::ABOUT_CNC, LCD_ARROW_COL, LCD_ZERO_ROW, "About CNC", 14);

  MAIN_SCREEN.hide(MainOption::NO_TF_CARD);
  MAIN_SCREEN.hide(MainOption::PAUSE_PROC);
  MAIN_SCREEN.hide(MainOption::RESUME_PROC);
  MAIN_SCREEN.hide(MainOption::STOP_PROC);
}

void setupMoveAxesScreen()
{
  createLine(MOVE_AXES_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Prepare", 8);
  createLine(MOVE_AXES_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X", 8);
  createLine(MOVE_AXES_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y", 8);
  createLine(MOVE_AXES_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z", 8);
}

void setupMoveAxisScreen()
{
  createLine(MOVE_AXIS_SCREEN, 0, LCD_ZERO_ROW, 1, "Move %s", 0);
  createLine(MOVE_AXIS_SCREEN, 1, LCD_ZERO_ROW, 2, "+%03d.%d", 0);
}

void setupMoveXScreen()
{

  createLine(MOVE_X_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes", 13);
  createLine(MOVE_X_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 10mm", 13);
  createLine(MOVE_X_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 1mm", 13);
  createLine(MOVE_X_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 0.1mm", 13);
}

void setupMoveYScreen()
{

  createLine(MOVE_Y_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes", 13);
  createLine(MOVE_Y_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 10mm", 13);
  createLine(MOVE_Y_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 1mm", 13);
  createLine(MOVE_Y_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 0.1mm", 13);
}

void setupMoveZScreen()
{
  createLine(MOVE_Z_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes", 13);
  createLine(MOVE_Z_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 10mm", 13);
  createLine(MOVE_Z_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 1mm", 13);
  createLine(MOVE_Z_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 0.1mm", 13);
}

void setupPrepScreen()
{
  createLine(PREP_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Main", 17);
  createLine(PREP_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "Move axes", 17);
  createLine(PREP_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Auto home", 17);
  createLine(PREP_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Set home offsets", 17);
  createLine(PREP_SCREEN, 4, LCD_ARROW_COL, LCD_ZERO_ROW, "Disable steppers", 17);
}

void setupMotionScreen()
{
  createLine(MOTION_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Control", 13);
  createLine(MOTION_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "Acceleration", 13);
  createLine(MOTION_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Velocity", 13);
  createLine(MOTION_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Steps/mm", 13);
}

void setupStepsScreen()
{
  createLine(STEPS_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion", 11);
  createLine(STEPS_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "X steps/mm", 11);
  createLine(STEPS_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Y steps/mm", 11);
  createLine(STEPS_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Z steps/mm", 11);
}

void setupVelocityScreen()
{
  createLine(VELOCITY_SCREEN, 0, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion", 7);
  createLine(VELOCITY_SCREEN, 1, LCD_ARROW_COL, LCD_ZERO_ROW, "Vel", 7);
  createLine(VELOCITY_SCREEN, 2, LCD_ARROW_COL, LCD_ZERO_ROW, "Travel", 7);
  createLine(VELOCITY_SCREEN, 3, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax X", 7);
  createLine(VELOCITY_SCREEN, 4, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax Y", 7);
  createLine(VELOCITY_SCREEN, 5, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax Z", 7);
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
    commands.push(std::make_shared<LinearMoveCommand>(cartesian, laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'F', 0), parseNumber(line, 'S', 0)));
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
    commands.push(std::make_shared<AutohomeCommand>(cartesian, laser));
    break;
  default:
    break;
  }
}

void completeProcess()
{
  if (commands.empty())
  {
    cartesian.disableSteppers();
    proc = nullptr;

    displayInfoScreen();
  }
}

void executeProcess()
{
  parseNextCommand();

  if (!commands.empty() && !cmd)
  {
    cmd = commands.front();
    commands.pop();

    if (cmd)
    {
      cmd->start();
    }
  }

  if (cmd)
  {
    Status status = cmd->status();

    switch (status)
    {
    case Status::COMPLETED:
      cmd->finish();
      cmd = nullptr;
      break;

    case Status::CONTINUE:
      if (proc->status() == Status::CONTINUE)
      {
        cmd->execute();
      }
      break;

    case Status::ERROR:
      cmd = nullptr;
      break;
    default:
      break;
    }
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
  Wire.setClock(3400000); // High Speed Mode at 3.4 Mbps

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.createChar(LCD_ARROW_CHAR, ARROW_CHAR);
  lcd.createChar(LCD_GAUGE_FILL_1, GAUGE_FILL_1);
  lcd.createChar(LCD_GAUGE_FILL_2, GAUGE_FILL_2);
  lcd.createChar(LCD_GAUGE_FILL_3, GAUGE_FILL_3);
  lcd.createChar(LCD_GAUGE_FILL_4, GAUGE_FILL_4);
  lcd.createChar(LCD_GAUGE_LEFT, GAUGE_LEFT);
  lcd.createChar(LCD_GAUGE_RIGHT, GAUGE_RIGHT);
  lcd.createChar(LCD_GAUGE_EMPTY, GAUGE_EMPTY);

  root = SD.open("/");

  stepperX->setMaxSpeed(400);
  stepperY->setMaxSpeed(400);
  stepperZ->setMaxSpeed(400);

  stepperX->setAcceleration(300);
  stepperY->setAcceleration(300);
  stepperZ->setAcceleration(300);

  switchX->setDebounceTime(10);
  switchY->setDebounceTime(10);
  switchZ->setDebounceTime(10);

  laser.setMaxPower(LSR_MAX_POWER);

  rotary.setOnClicked(clickCallback);
  rotary.setOnRotation(rotateCallback);

  rotary.setClickDebounceTime(EN_CLICK_DEBOUNCE);
  rotary.setRotationDebounceTime(EN_ROT_DEBOUNCE);

  cartesian.setStepperMotor(Axis::X, stepperX);
  cartesian.setStepperMotor(Axis::Y, stepperY);
  cartesian.setStepperMotor(Axis::Z, stepperZ);

  cartesian.setLimitSwitch(Axis::X, switchX);
  cartesian.setLimitSwitch(Axis::Y, switchY);
  cartesian.setLimitSwitch(Axis::Z, switchZ);

  cartesian.setMinStepsPerMillimeter(Axis::X, 5);
  cartesian.setMinStepsPerMillimeter(Axis::Y, 5);
  cartesian.setMinStepsPerMillimeter(Axis::Z, 25);

  cartesian.setStepsPerMillimeter(Axis::X, 10);
  cartesian.setStepsPerMillimeter(Axis::Y, 10);
  cartesian.setStepsPerMillimeter(Axis::Z, 50);

  cartesian.setDimension(Axis::X, 400);
  cartesian.setDimension(Axis::Y, 420);
  cartesian.setDimension(Axis::Z, 95);

  setupAccelScreen();
  setupAboutScreen();
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
      proc->start();

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
      completeProcess();
      break;

    case Status::CONTINUE:
      executeProcess();
      break;

    case Status::ERROR:
      proc = nullptr;
      break;
    default:
      break;
    }
  }
}
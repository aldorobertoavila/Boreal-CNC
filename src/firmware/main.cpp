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

#define LCD_ARROW_COL 2
#define LCD_ZERO_COL 0
#define LCD_ZERO_ROW 0

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

String READY_MSG("Ready!         00:00");
String MOTOR_MSG("X0 Y0 Z0            ");

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

LiquidLinePtr positionInfo;
LiquidLinePtr processInfo;
LiquidLinePtr progressInfo;

LiquidLinePtr noTFCard;
LiquidLinePtr startFromTF;
LiquidLinePtr pauseOption;
LiquidLinePtr resumeOption;
LiquidLinePtr stopOption;

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

void clearRow(LiquidLinePtr line)
{
  uint8_t row = line->getRow();

  lcd.setCursor(0, row);
  lcd.print(EMPTY_ROW);
}

void updateProcessInfo(String time)
{
  String name = proc->name();

  if (name.length() > GAUGE_SIZE)
  {
    name = name.substring(0, GAUGE_SIZE);
  }
  else
  {
    name = pad_right(name.c_str(), GAUGE_SIZE).c_str();
  }

  processInfo->setText(name + " " + time);
  processInfo->display(lcd);
}

void updateProgressInfo(uint8_t progress)
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

  progressInfo->setText(String(gaugeBuffer) + " " + progress + "%");
  progressInfo->display(lcd);
}

void updateInfoScreen()
{
  if (currentScreen != ScreenID::INFO)
  {
    return;
  }

  char positionBuf[LCD_COLS];

  float x = cartesian.getTargetPosition(Axis::X);
  float y = cartesian.getTargetPosition(Axis::Y);
  float z = cartesian.getTargetPosition(Axis::Z);

  switch (cartesian.getUnit())
  {
  case Unit::INCH:
    snprintf(positionBuf, LCD_COLS, "X%.1f Y%.1f Z%.1f", x, y, z);
    break;
  case Unit::MILLIMETER:
    snprintf(positionBuf, LCD_COLS, "X%i Y%i Z%i", (int)x, (int)y, (int)z);
    break;
  default:
    break;
  }

  if (positionInfo->getText() != positionBuf)
  {
    clearRow(positionInfo);
    positionInfo->setText(positionBuf);
    positionInfo->display(lcd);
  }

  String prevTime = proc->getPreviousTime();
  String time = proc->getTime();

  if (prevTime != time)
  {
    updateProcessInfo(time);
  }

  uint8_t prevProgress = proc->getPreviousProgress();
  uint8_t progress = proc->getProgress();

  if (progress != prevProgress)
  {
    updateProgressInfo(progress);
  }
}

void display(ScreenID screenIndex, bool forcePosition)
{
  LiquidScreen *screen = getScreen(screenIndex);

  if (screen)
  {
    uint8_t lineIndex = screen->getCurrentLineIndex();
    uint8_t lineCount = screen->getLineCount();

    rotary.setBoundaries(0, lineCount);
    rotary.setPosition(forcePosition ? lineIndex : 0);

    currentScreen = screenIndex;
    screen->display();
  }
}

void displayInfoScreen()
{
  display(ScreenID::INFO, true);
  updateProgressInfo(proc ? proc->getProgress() : 0);
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
    Serial.println(path);

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
    processes.push(std::make_shared<Process>(SD, rtc, "/" + filePaths[index - 1]));
    cartesian.enableSteppers();
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
  case 0:
    displayInfoScreen();
    break;
  case 1:
    display(ScreenID::PREP, false);
    break;
  case 2:
    display(ScreenID::CTRL, false);
    break;
  case 3:
    display(ScreenID::CARD, false);
    break;
  case 4:
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

LiquidLinePtr createLine(LiquidScreen &screen, uint8_t row, uint8_t col, String text, size_t textLength)
{
  LiquidLinePtr ptr = std::make_shared<LiquidLine>(row, col, pad_right(text.c_str(), textLength).c_str());

  screen.append(ptr);
  return ptr;
}

void setupAccelScreen()
{
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion", 7);
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Accel", 7);
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Travel", 7);
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax X", 7);
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax Y", 7);
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax Z", 7);
}

void setupAboutScreen()
{
  createLine(ABOUT_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Main", 4);
}

void setupCardScreen()
{
  createLine(CARD_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Main", 4);

  uint8_t n = getFiles(root, filePaths, MAX_FILES_SIZE);

  for (uint8_t i = 0; i < n; i++)
  {
    createLine(CARD_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, filePaths[i].c_str(), 18);
  }
}

void setupCtrlScreen()
{
  createLine(CTRL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Main", 15);
  createLine(CTRL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion", 15);
  createLine(CTRL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Laser", 15);
  createLine(CTRL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Store settings", 15);
}

void setupInfoScreen()
{
  createLine(INFO_SCREEN, LCD_ZERO_COL, 0, "Boreal CNC", 0);
  positionInfo = createLine(INFO_SCREEN, LCD_ZERO_COL, 1, MOTOR_MSG, 0);
  processInfo = createLine(INFO_SCREEN, LCD_ZERO_COL, 2, READY_MSG, 0);
  progressInfo = createLine(INFO_SCREEN, LCD_ZERO_COL, 3, "0%", 0);
}

void setupMainScreen()
{
  createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Info", 14);
  createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Prepare", 14);
  createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Control", 14);
  startFromTF = createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Start from TF", 14);
  createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "About CNC", 14);
}

void setupMoveAxesScreen()
{
  createLine(MOVE_AXES_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Prepare", 8);
  createLine(MOVE_AXES_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X", 8);
  createLine(MOVE_AXES_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y", 8);
  createLine(MOVE_AXES_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z", 8);
}

void setupMoveAxisScreen()
{
  createLine(MOVE_AXIS_SCREEN, LCD_ZERO_ROW, 1, "Move %s", 0);
  createLine(MOVE_AXIS_SCREEN, LCD_ZERO_ROW, 2, "+%03d.%d", 0);
}

void setupMoveXScreen()
{

  createLine(MOVE_X_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes", 13);
  createLine(MOVE_X_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 10mm", 13);
  createLine(MOVE_X_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 1mm", 13);
  createLine(MOVE_X_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 0.1mm", 13);
}

void setupMoveYScreen()
{

  createLine(MOVE_Y_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes", 13);
  createLine(MOVE_Y_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 10mm", 13);
  createLine(MOVE_Y_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 1mm", 13);
  createLine(MOVE_Y_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 0.1mm", 13);
}

void setupMoveZScreen()
{
  createLine(MOVE_Z_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes", 13);
  createLine(MOVE_Z_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 10mm", 13);
  createLine(MOVE_Z_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 1mm", 13);
  createLine(MOVE_Z_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 0.1mm", 13);
}

void setupPrepScreen()
{

  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Main", 17);
  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move axes", 17);
  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Auto home", 17);
  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Set home offsets", 17);
  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Disable steppers", 17);
}

void setupMotionScreen()
{
  createLine(MOTION_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Control", 13);
  createLine(MOTION_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Acceleration", 13);
  createLine(MOTION_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Velocity", 13);
  createLine(MOTION_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Steps/mm", 13);
}

void setupStepsScreen()
{
  createLine(STEPS_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion", 11);
  createLine(STEPS_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "X steps/mm", 11);
  createLine(STEPS_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Y steps/mm", 11);
  createLine(STEPS_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Z steps/mm", 11);
}

void setupVelocityScreen()
{
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion", 7);
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Vel", 7);
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Travel", 7);
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax X", 7);
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax Y", 7);
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax Z", 7);
}

void parseNextCommand()
{
  if (commands.size() >= CMD_QUEUE_SIZE)
  {
    return;
  }

  String line = proc.get()->readNextLine();

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

  stepperX->setMaxSpeed(600);
  stepperY->setMaxSpeed(600);
  stepperZ->setMaxSpeed(600);

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
      updateProcessInfo("00:00");
      updateProgressInfo(0);
    }
  }

  if (proc)
  {
    updateInfoScreen();

    switch (proc->status())
    {
    case Status::COMPLETED:
      cartesian.disableSteppers();

      clearRow(positionInfo);
      clearRow(processInfo);
      clearRow(progressInfo);
      positionInfo->setText(MOTOR_MSG);
      positionInfo->display(lcd);
      processInfo->setText(READY_MSG);
      processInfo->display(lcd);
      updateProgressInfo(0);
      proc = nullptr;
      break;

    case Status::CONTINUE:
      parseNextCommand();
      break;

    case Status::ERROR:
      proc = nullptr;
      break;
    default:
      break;
    }
  }

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
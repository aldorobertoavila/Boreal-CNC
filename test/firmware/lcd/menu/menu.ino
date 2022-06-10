#include <Arduino.h>
#include <LiquidViewport.h>
#include <Rotary.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define EN_DT_PIN 2
#define EN_SW_PIN 3
#define EN_CLK_PIN 4

#define EN_CLICK_DEBOUNCE 500
#define EN_ROT_DEBOUNCE 100

#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

#define ARROW_CHAR 0

#define ARROW_COL_OFFSET 2
#define ZERO_COL_OFFSET 0
#define ZERO_ROW_OFFSET 0
#define AXIS_COL_OFFSET 7

enum Axis
{
  X,
  Y,
  Z
};

enum Screen
{
  INFO,
  MAIN,
  PREP,
  CTRL,
  CARD,
  ABOUT,
  MOVE_AXES,
  MOVE_AXIS,
  MOVE_X,
  MOVE_Y,
  MOVE_Z
};

const uint8_t TEN_MILLIMETER = 100;
const uint8_t ONE_MILLIMETER = 10;
const uint8_t TENTH_MILLIMETER = 1;

Axis currentAxis;
uint8_t currentUnit;
uint16_t millimeters;

byte ARROW[8] =
{
  0b00000,
  0b00100,
  0b00110,
  0b11111,
  0b11111,
  0b00110,
  0b00100,
  0b00000
};

LCD lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

LiquidLine headerLine(0, 0, "Boreal CNC");
LiquidLine motorsLine(0, 1, "X: 0 Y: 0 Z: 0");
LiquidLine progressLine(0, 2, "0%");
LiquidLine statusLine(0, 3, "CNC ready.");

LiquidLine infoLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Info   ");
LiquidLine prepLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Prepare");
LiquidLine ctrlLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Control   ");
LiquidLine cardLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "No TF card");
LiquidLine aboutLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "About CNC ");

LiquidLine mainLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Main     ");
LiquidLine moveAxisLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move axes");
LiquidLine autoHomeLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Auto home       ");
LiquidLine setHomeLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Set home offsets");
LiquidLine disableLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Disable steppers");

LiquidLine moveXLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move X");
LiquidLine moveYLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Y");
LiquidLine moveZLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Z");

LiquidLine moveLine(AXIS_COL_OFFSET, 1, "Move %s"); // "Move X"
LiquidLine milliLine(AXIS_COL_OFFSET, 2, "+%03d.%d"); // "+000.0"

LiquidLine moveX10mmLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move X 10mm");
LiquidLine moveX1mmLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move X 1mm");
LiquidLine moveX01mmLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move X 0.1mm");

LiquidLine moveY10mmLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Y 10mm");
LiquidLine moveY1mmLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Y 1mm");
LiquidLine moveY01mmLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Y 0.1mm");

LiquidLine moveZ10mmLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Z 10mm");
LiquidLine moveZ1mmLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Z 1mm");
LiquidLine moveZ01mmLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Z 0.1mm");

LiquidScreen infoScreen;
LiquidScreen aboutScreen;
LiquidScreen moveAxisScreen;

LiquidMenu mainScreen;
LiquidMenu prepScreen;
LiquidMenu ctrlScreen;
LiquidMenu cardScreen;

LiquidMenu moveAxesScreen;
LiquidMenu moveXScreen;
LiquidMenu moveYScreen;
LiquidMenu moveZScreen;

LiquidViewport viewport(lcd, LCD_COLS, LCD_ROWS);
Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

void autohome()
{
  Serial.println("Auto home");
}

void setHomeOffsets()
{
  Serial.println("Set Home Offsets");
}

void disableSteppers()
{
  Serial.println("Disable Steppers");
}

void setScreen(uint8_t screenId)
{
  setScreen(screenId, false);
}

void setScreen(LiquidScreen *screen, uint8_t screenId, bool forcePosition)
{
  rotary.setLowerBound(0);
  rotary.setUpperBound(screen->getLineCount());
  rotary.forcePosition(forcePosition ? viewport.getCurrentLineIndex(screenId) : 0);

  viewport.setCurrentScreen(screenId);
  viewport.display(true);
}

void setScreen(uint8_t screenId, bool forcePosition)
{
  LiquidScreen *screen = viewport.getScreen(screenId);

  if (screen)
  {
    setScreen(screen, screenId, forcePosition);
  }
}

void setMoveAxisScreen(Axis axis, uint8_t unit) {
  setScreen(MOVE_AXIS);
  currentAxis = axis;
  currentUnit = unit;

  // TODO: lower bound from cnc max increment of unit
  // TEN_MILLIMETER -> 100
  // ONE_MILLIMETER -> 1,000
  // TENTH_MILLIMETER -> 10,000
  // [-100, lower) to reverse direction.

  // upper bound cnc max decrement of unit if n > unit 
  // rotary.setLowerBound(-100);
  // rotary.setUpperBound(1);

  // rotary won't change
  rotary.setLowerBound(0);
  rotary.setUpperBound(1);
  viewport.setCurrentScreen(MOVE_AXIS);
  viewport.display(true);
}

void infoScreenClicked()
{
  setScreen(MAIN);
}

void mainScreenClicked()
{

  switch (viewport.getCurrentLineIndex(MAIN))
  {
  case 0:
    setScreen(INFO);
    break;
  case 1:
    setScreen(PREP);
    break;
  case 2:
    setScreen(CTRL);
    break;
  case 3:
    /*
      setScreen(CARD);
    */
    break;
  case 4:
    setScreen(ABOUT);
    break;
  default:
    break;
  }
}

void prepScreenClicked()
{
  switch (viewport.getCurrentLineIndex(PREP))
  {
  case 0:
    setScreen(MAIN, true);
    break;
  case 1:
    setScreen(MOVE_AXES);
    break;
  case 2:
    autohome();
    break;
  case 3:
    setHomeOffsets();
    break;
  case 4:
    disableSteppers();
    break;
  default:
    break;
  }
}

void ctrlScreenClicked()
{
  switch (viewport.getCurrentLineIndex(CTRL))
  {
  case 0:
    setScreen(MAIN, true);
    break;
  default:
    break;
  }
}

void cardScreenClicked()
{
  switch (viewport.getCurrentLineIndex(CARD))
  {
  case 0:
    setScreen(MAIN, true);
    break;
  default:
    break;
  }
}

void aboutScreenClicked()
{
  setScreen(MAIN, true);
}

void moveAxesScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOVE_AXES))
  {
  case 0:
    setScreen(PREP, true);
    break;
  case 1:
    setScreen(MOVE_X);
    break;
  case 2:
    setScreen(MOVE_Y);
    break;
  case 3:
    setScreen(MOVE_Z);
    break;
  default:
    break;
  }
}

void moveAxisScreenClicked()
{
  switch (currentAxis)
  {
  case X:
    setScreen(MOVE_X, true);
    break;
  case Y:
    setScreen(MOVE_Y, true);
    break;
  case Z:
    setScreen(MOVE_Z, true);
    break;
  default:
    break;
  }
}

void moveXScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOVE_X))
  {
  case 0:
    setScreen(MOVE_AXES, true);
    break;
  case 1:
    setMoveAxisScreen(X, TEN_MILLIMETER);
    break;
  case 2:
    setMoveAxisScreen(X, ONE_MILLIMETER);
    break;
  case 3:
    setMoveAxisScreen(X, TENTH_MILLIMETER);
    break;
  default:
    break;
  }
}

void moveYScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOVE_Y))
  {
  case 0:
    setScreen(MOVE_AXES, true);
    break;
  case 1:
    setMoveAxisScreen(Y, TEN_MILLIMETER);
    break;
  case 2:
    setMoveAxisScreen(Y, ONE_MILLIMETER);
    break;
  case 3:
    setMoveAxisScreen(Y, TENTH_MILLIMETER);
    break;
  default:
    break;
  }
}

void moveZScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOVE_Z))
  {
  case 0:
    setScreen(MOVE_AXES, true);
    break;
  case 1:
    setMoveAxisScreen(Z, TEN_MILLIMETER);
    break;
  case 2:
    setMoveAxisScreen(Z, ONE_MILLIMETER);
    break;
  case 3:
    setMoveAxisScreen(Z, TENTH_MILLIMETER);
    break;
  default:
    break;
  }
}

void onClicked()
{
  switch (viewport.getCurrentScreenIndex())
  {
  case INFO:
    infoScreenClicked();
    break;
  case MAIN:
    mainScreenClicked();
    break;
  case PREP:
    prepScreenClicked();
    break;
  case CTRL:
    ctrlScreenClicked();
    break;
  case CARD:
    cardScreenClicked();
    break;
  case ABOUT:
    aboutScreenClicked();
    break;
  case MOVE_AXES:
    moveAxesScreenClicked();
    break;
  case MOVE_AXIS:
    moveAxisScreenClicked();
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
  default:
    break;
  }
}

void onRotationCCW()
{
  switch (viewport.getCurrentScreenIndex())
  {
    case MAIN:
    case PREP:
    case MOVE_AXES:
    case MOVE_X:
    case MOVE_Y:
    case MOVE_Z:
      viewport.previousLine();
      viewport.display(false);
      break;
    case MOVE_AXIS:
      // cnc.move(currentAxis, Direction::POSITIVE, units);
      millimeters+=currentUnit;
      viewport.display(true);
      break;
    default:
      break;
  }
}

void onRotationCW()
{
  switch (viewport.getCurrentScreenIndex())
  {
    case MAIN:
    case PREP:
    case MOVE_AXES:
    case MOVE_X:
    case MOVE_Y:
    case MOVE_Z:
      viewport.nextLine();
      viewport.display(false);
      break;
    case MOVE_AXIS:
      // cnc.move(currentAxis, Direction::NEGATIVE, units);
      millimeters-=currentUnit;
      if (millimeters < 0) millimeters = 0;
      viewport.display(true);
      break;
    default:
      break;
  }
}

void formatAxisLine(Buffer buf, char* text)
{
  switch (currentAxis)
  {
    case X:
      snprintf(buf, sizeof(buf), text, "X");
      break;
    case Y:
      snprintf(buf, sizeof(buf), text, "Y");
      break;
    case Z:
      snprintf(buf, sizeof(buf), text, "Z");
      break;
  }
}

void formatMilliLine(Buffer buf, char* text)
{
  uint8_t tens = millimeters / 10;
  uint8_t ones = millimeters % 10;
  snprintf(buf, sizeof(buf), text, tens, ones);
}

void setup()
{
  Serial.begin(115200);

  pinMode(EN_CLK_PIN, INPUT);
  pinMode(EN_DT_PIN, INPUT);
  pinMode(EN_SW_PIN, INPUT_PULLUP);

  rotary.onClicked(onClicked);
  rotary.onRotationCCW(onRotationCCW);
  rotary.onRotationCW(onRotationCW);

  rotary.setClickDebounceTime(EN_CLICK_DEBOUNCE);
  rotary.setRotationDebounceTime(EN_ROT_DEBOUNCE);

  lcd.init();
  Wire.setClock(400000);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.createChar(ARROW_CHAR, ARROW);

  infoScreen.addLine(0, headerLine);
  infoScreen.addLine(1, motorsLine);
  infoScreen.addLine(2, progressLine);
  infoScreen.addLine(3, statusLine);

  mainScreen.addLine(0, infoLine);
  mainScreen.addLine(1, prepLine);
  mainScreen.addLine(2, ctrlLine);
  mainScreen.addLine(3, cardLine);
  mainScreen.addLine(4, aboutLine);

  prepScreen.addLine(0, mainLine);
  prepScreen.addLine(1, moveAxisLine);
  prepScreen.addLine(2, autoHomeLine);
  prepScreen.addLine(3, setHomeLine);
  prepScreen.addLine(4, disableLine);

  moveAxesScreen.addLine(0, prepLine);
  moveAxesScreen.addLine(1, moveXLine);
  moveAxesScreen.addLine(2, moveYLine);
  moveAxesScreen.addLine(3, moveZLine);

  moveXScreen.addLine(0, moveAxisLine);
  moveXScreen.addLine(1, moveX10mmLine);
  moveXScreen.addLine(2, moveX1mmLine);
  moveXScreen.addLine(3, moveX01mmLine);

  moveYScreen.addLine(0, moveAxisLine);
  moveYScreen.addLine(1, moveY10mmLine);
  moveYScreen.addLine(2, moveY1mmLine);
  moveYScreen.addLine(3, moveY01mmLine);

  moveZScreen.addLine(0, moveAxisLine);
  moveZScreen.addLine(1, moveZ10mmLine);
  moveZScreen.addLine(2, moveZ1mmLine);
  moveZScreen.addLine(3, moveZ01mmLine);

  moveAxisScreen.addLine(0, moveLine);
  moveAxisScreen.addLine(1, milliLine);

  moveLine.setFormat(formatAxisLine);
  milliLine.setFormat(formatMilliLine);

  ctrlScreen.addLine(0, mainLine);
  cardScreen.addLine(0, mainLine);
  aboutScreen.addLine(0, mainLine);

  viewport.addScreen(INFO, infoScreen);
  viewport.addScreen(MAIN, mainScreen);
  viewport.addScreen(PREP, prepScreen);
  viewport.addScreen(CTRL, ctrlScreen);
  viewport.addScreen(CARD, cardScreen);
  viewport.addScreen(ABOUT, aboutScreen);

  viewport.addScreen(MOVE_AXES, moveAxesScreen);
  viewport.addScreen(MOVE_AXIS, moveAxisScreen);
  viewport.addScreen(MOVE_X, moveXScreen);
  viewport.addScreen(MOVE_Y, moveYScreen);
  viewport.addScreen(MOVE_Z, moveZScreen);

  viewport.setCurrentScreen(INFO);
  viewport.display(true);
}

void loop()
{
  rotary.tick();
}
#include <Arduino.h>
#include <LiquidViewport.h>
#include <Rotary.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define EN_DT_PIN 2
#define EN_SW_PIN 3
#define EN_CLK_PIN 4

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

const uint32_t BAUD_RATE = 115200;
const uint32_t CLOCK_FREQ = 400000;

enum Screen
{
  INFO,
  MAIN,
  PREP,
  CTRL,
  CARD,
  ABOUT,
  MOVE_AXIS,
  MOVE_X,
  MOVE_Y,
  MOVE_Z
};

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

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
LiquidLine moveAxisLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move axis");
LiquidLine autoHomeLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Auto home       ");
LiquidLine setHomeLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Set home offsets");
LiquidLine disableLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Disable steppers");

LiquidLine moveXLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move X");
LiquidLine moveYLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Y");
LiquidLine moveZLine(ARROW_COL_OFFSET, ZERO_ROW_OFFSET, "Move Z");

LiquidScreen infoScreen;
LiquidScreen aboutScreen;
LiquidMenu mainScreen;
LiquidMenu prepScreen;
LiquidMenu ctrlScreen;
LiquidMenu cardScreen;

LiquidMenu moveAxisScreen;
LiquidMenu moveXScreen;
LiquidMenu moveYScreen;
LiquidMenu moveZScreen;

LiquidViewport viewport(lcd, LCD_COLS, LCD_ROWS);
Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

void setScreen(uint8_t screenId)
{
  setScreen(screenId, false);
}

void setScreen(uint8_t screenId, bool forcePosition)
{
  LiquidScreen *screen = viewport.getScreen(screenId);

  if (screen)
  {
    rotary.setLowerBound(0);
    rotary.setUpperBound(screen->getLineCount());
    rotary.forcePosition(forcePosition ? viewport.getCurrentLineIndex(screenId) : 0);

    viewport.setCurrentScreen(screenId);
    viewport.display();
  }
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
    setScreen(MOVE_AXIS);
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

void moveAxisScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOVE_AXIS))
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

void moveXScreenClicked()
{
  setScreen(MOVE_AXIS, true);
}

void moveYScreenClicked()
{
  setScreen(MOVE_AXIS, true);
}

void moveZScreenClicked()
{
  setScreen(MOVE_AXIS, true);
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
  case MOVE_AXIS:
    viewport.previousLine();
    viewport.display();
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
  case MOVE_AXIS:
    viewport.nextLine();
    viewport.display();
    break;
  default:
    break;
  }
}

void setup()
{
  Serial.begin(BAUD_RATE);
  Wire.begin();
  Wire.setClock(CLOCK_FREQ);

  pinMode(EN_CLK_PIN, INPUT);
  pinMode(EN_DT_PIN, INPUT);
  pinMode(EN_SW_PIN, INPUT_PULLUP);

  rotary.onClicked(onClicked);
  rotary.onRotationCCW(onRotationCCW);
  rotary.onRotationCW(onRotationCW);

  rotary.setClickDebounceTime(EN_CLICK_DEBOUNCE);
  rotary.setRotationDebounceTime(EN_ROT_DEBOUNCE);

  lcd.init();
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

  moveAxisScreen.addLine(0, prepLine);
  moveAxisScreen.addLine(1, moveXLine);
  moveAxisScreen.addLine(2, moveYLine);
  moveAxisScreen.addLine(3, moveZLine);

  moveXScreen.addLine(0, moveAxisLine);
  moveYScreen.addLine(0, moveAxisLine);
  moveZScreen.addLine(0, moveAxisLine);

  ctrlScreen.addLine(0, mainLine);
  cardScreen.addLine(0, mainLine);
  aboutScreen.addLine(0, mainLine);

  viewport.addScreen(INFO, infoScreen);
  viewport.addScreen(MAIN, mainScreen);
  viewport.addScreen(PREP, prepScreen);
  viewport.addScreen(CTRL, ctrlScreen);
  viewport.addScreen(CARD, cardScreen);
  viewport.addScreen(ABOUT, aboutScreen);

  viewport.addScreen(MOVE_AXIS, moveAxisScreen);
  viewport.addScreen(MOVE_X, moveXScreen);
  viewport.addScreen(MOVE_Y, moveYScreen);
  viewport.addScreen(MOVE_Z, moveZScreen);

  viewport.setCurrentScreen(INFO);
  viewport.display();
}

void loop()
{
  rotary.tick();
}
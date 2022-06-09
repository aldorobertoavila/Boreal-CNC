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

const uint8_t INFO_SCREEN = 0;
const uint8_t MAIN_SCREEN = 1;
const uint8_t PREP_SCREEN = 2;
const uint8_t CTRL_SCREEN = 3;
const uint8_t CARD_SCREEN = 4;
const uint8_t ABOUT_SCREEN = 5;

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

LiquidScreen infoScreen;
LiquidScreen aboutScreen;
LiquidMenu prepScreen;
LiquidMenu ctrlScreen;
LiquidMenu cardScreen;
LiquidMenu mainScreen;

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
  setScreen(MAIN_SCREEN);
}

void mainScreenClicked()
{

  switch (viewport.getCurrentLineIndex(MAIN_SCREEN))
  {
  case 0:
    setScreen(INFO_SCREEN);
    break;
  case 1:
    setScreen(PREP_SCREEN);
    break;
  case 2:
    setScreen(CTRL_SCREEN);
    break;
  case 3:
    /*
      setScreen(CARD_SCREEN);
    */
    break;
  case 4:
    setScreen(ABOUT_SCREEN);
    break;
  default:
    break;
  }
}

void prepScreenClicked()
{
  switch (viewport.getCurrentLineIndex(PREP_SCREEN))
  {
  case 0:
    setScreen(MAIN_SCREEN, true);
    break;
  default:
    break;
  }
}

void ctrlScreenClicked()
{
  switch (viewport.getCurrentLineIndex(CTRL_SCREEN))
  {
  case 0:
    setScreen(MAIN_SCREEN, true);
    break;
  default:
    break;
  }
}

void cardScreenClicked()
{
  switch (viewport.getCurrentLineIndex(CARD_SCREEN))
  {
  case 0:
    setScreen(MAIN_SCREEN, true);
    break;
  default:
    break;
  }
}

void aboutScreenClicked()
{
  setScreen(MAIN_SCREEN, viewport.getCurrentLineIndex(MAIN_SCREEN));
}

void onClicked()
{
  switch (viewport.getCurrentScreenIndex())
  {
  case INFO_SCREEN:
    infoScreenClicked();
    break;
  case MAIN_SCREEN:
    mainScreenClicked();
    break;
  case PREP_SCREEN:
    prepScreenClicked();
    break;
  case CTRL_SCREEN:
    ctrlScreenClicked();
    break;
  case CARD_SCREEN:
    cardScreenClicked();
    break;
  case ABOUT_SCREEN:
    aboutScreenClicked();
    break;
  default:
    break;
  }
}

void onRotationCCW()
{
  switch (viewport.getCurrentScreenIndex())
  {
  case MAIN_SCREEN:
  case PREP_SCREEN:
  case CTRL_SCREEN:
  case CARD_SCREEN:
  case ABOUT_SCREEN:
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
  case MAIN_SCREEN:
  case PREP_SCREEN:
  case CTRL_SCREEN:
  case CARD_SCREEN:
  case ABOUT_SCREEN:
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

  ctrlScreen.addLine(0, mainLine);
  cardScreen.addLine(0, mainLine);
  aboutScreen.addLine(0, mainLine);

  viewport.addScreen(INFO_SCREEN, infoScreen);
  viewport.addScreen(MAIN_SCREEN, mainScreen);
  viewport.addScreen(PREP_SCREEN, prepScreen);
  viewport.addScreen(CTRL_SCREEN, ctrlScreen);
  viewport.addScreen(CARD_SCREEN, cardScreen);
  viewport.addScreen(ABOUT_SCREEN, aboutScreen);

  viewport.setCurrentScreen(INFO_SCREEN);
  viewport.display();
}

void loop()
{
  rotary.tick();
}
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

#define EN_CLICK_DEBOUNCE 100
#define EN_ROT_DEBOUNCE 10

#define EN_LI 0
#define EN_LS 4

#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

#define ARROW_CHAR 0
#define ARROW_OFFSET 2

byte ARROW[8] =
    {
        0b00000,
        0b00100,
        0b00110,
        0b11111,
        0b11111,
        0b00110,
        0b00100,
        0b00000};

const uint8_t INFO_SCREEN = 0;
const uint8_t MAIN_SCREEN = 1;

const uint8_t MAIN_OPTIONS = 5;

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

LiquidViewport viewport(lcd, LCD_COLS, LCD_ROWS);
Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

LiquidScreen infoScreen;
LiquidScreen aboutScreen;

LiquidMenu mainScreen;
LiquidMenu prepScreen;
LiquidMenu ctrlScreen;
LiquidMenu cardScreen;

LiquidLine headerLine(0, 0, "Boreal CNC");
LiquidLine motorsLine(0, 1, "X: 0 Y: 0 Z: 0");
LiquidLine progressLine(0, 2, "0%");
LiquidLine statusLine(0, 3, "CNC ready.");

LiquidLine infoLine(ARROW_OFFSET, 0, "Info Screen");
LiquidLine prepLine(ARROW_OFFSET, 0, "Prepare");
LiquidLine ctrlLine(ARROW_OFFSET, 0, "Control");
LiquidLine cardLine(ARROW_OFFSET, 0, "No TF card");
LiquidLine aboutLine(ARROW_OFFSET, 0, "About CNC");

void infoClicked()
{
  viewport.setCurrentScreen(INFO_SCREEN);
  viewport.draw();
}

void prepClicked()
{
  Serial.println("Prepare clicked!");
}

void ctrlClicked()
{
  Serial.println("Control clicked!");
}

void cardClicked()
{
  Serial.println("Card clicked!");
}

void aboutClicked()
{
  Serial.println("About clicked!");
}

void onInfoClicked()
{
  viewport.setCurrentScreen(MAIN_SCREEN);
  viewport.draw();
}

void onMainClicked()
{
  switch (mainScreen.getCurrentIndex())
  {
  case 0:
    infoClicked();
    break;
  case 1:
    prepClicked();
    break;
  case 2:
    ctrlClicked();
    break;
  case 3:
    cardClicked();
    break;
  case 4:
    aboutClicked();
    break;
  }
}

void onClicked()
{
  switch (viewport.getCurrentIndex())
  {
  case INFO_SCREEN:
    onInfoClicked();
    break;
  case MAIN_SCREEN:
    onMainClicked();
    break;
  default:
    break;
  }
}

void onRotationChange(long pos, long prevPos)
{
  switch (viewport.getCurrentIndex())
  {
  case MAIN_SCREEN:
    mainScreen.setFocusPosition(0, pos % MAIN_OPTIONS - 1);
    break;
  default:
    break;
  }

  viewport.draw();
}

void onRotationCCW()
{
  switch (viewport.getCurrentIndex())
  {
  case MAIN_SCREEN:
    mainScreen.previousLine();
    break;
  default:
    break;
  }
}

void onRotationCW()
{
  switch (viewport.getCurrentIndex())
  {
  case MAIN_SCREEN:
    mainScreen.nextLine();
    break;
  default:
    break;
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(EN_CLK_PIN, INPUT);
  pinMode(EN_DT_PIN, INPUT);
  pinMode(EN_SW_PIN, INPUT_PULLUP);

  rotary.onClicked(onClicked);
  rotary.onRotationChange(onRotationChange);
  rotary.onRotationCCW(onRotationCCW);
  rotary.onRotationCW(onRotationCW);

  rotary.setClickDebounceTime(EN_CLICK_DEBOUNCE);
  rotary.setRotationDebounceTime(EN_ROT_DEBOUNCE);
  rotary.setLowerBound(EN_LI);
  rotary.setUpperBound(EN_LS);

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

  viewport.addScreen(INFO_SCREEN, infoScreen);
  viewport.addScreen(MAIN_SCREEN, mainScreen);

  viewport.setCurrentScreen(INFO_SCREEN);
  viewport.draw();
}

void loop()
{
  rotary.tick();
}
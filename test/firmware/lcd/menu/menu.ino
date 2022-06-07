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
#define EN_ROT_DEBOUNCE 100

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
  0b00000
};

const uint8_t INFO_SCREEN = 0;
const uint8_t MAIN_SCREEN = 1;
const uint8_t PREP_SCREEN = 2;
const uint8_t CTRL_SCREEN = 3;
const uint8_t CARD_SCREEN = 4;
const uint8_t ABOUT_SCREEN = 5;

const uint8_t MAIN_OPTIONS = 5;
const uint8_t PREP_OPTIONS = 5;
const uint8_t CTRL_OPTIONS = 1;
const uint8_t CARD_OPTIONS = 1;

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

LiquidLine headerLine(0, 0, "Boreal CNC");
LiquidLine motorsLine(0, 1, "X: 0 Y: 0 Z: 0");
LiquidLine progressLine(0, 2, "0%");
LiquidLine statusLine(0, 3, "CNC ready.");

LiquidLine infoLine(ARROW_OFFSET, 0, "Info");
LiquidLine prepLine(ARROW_OFFSET, 0, "Prepare");
LiquidLine ctrlLine(ARROW_OFFSET, 0, "Control");
LiquidLine cardLine(ARROW_OFFSET, 0, "No TF card");
LiquidLine aboutLine(ARROW_OFFSET, 0, "About CNC");

LiquidLine mainLine(ARROW_OFFSET, 0, "Main");

LiquidLine moveAxisLine(ARROW_OFFSET, 0, "Move axis");
LiquidLine autoHomeLine(ARROW_OFFSET, 0, "Auto home");
LiquidLine setHomeLine(ARROW_OFFSET, 0, "Set home offsets");
LiquidLine disableLine(ARROW_OFFSET, 0, "Disable steppers");

LiquidScreen infoScreen;
LiquidScreen aboutScreen;
LiquidMenu prepScreen;
LiquidMenu ctrlScreen;
LiquidMenu cardScreen;
LiquidMenu mainScreen;

LiquidViewport viewport(lcd, LCD_COLS, LCD_ROWS);
Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

void displayMainScreen()
{
  rotary.setUpperBound(MAIN_OPTIONS - 1);
  viewport.setCurrentScreen(MAIN_SCREEN);
  viewport.display(true);
}

void infoScreenClicked() {
  displayMainScreen();
}

void mainScreenClicked() {
  switch (mainScreen.getCurrentIndex())
  {
    case 0:  
      rotary.setUpperBound(0);
      viewport.setCurrentScreen(INFO_SCREEN);
      viewport.display();
      break;
    case 1:
      rotary.setUpperBound(PREP_OPTIONS - 1);
      viewport.setCurrentScreen(PREP_SCREEN);
      viewport.display();
      break;
    case 2:
      rotary.setUpperBound(0);
      viewport.setCurrentScreen(CTRL_SCREEN);
      viewport.display(true);
      break;
    case 3:
      /*
      rotary.setUpperBound(0);
      viewport.setCurrentScreen(CARD_SCREEN);
      viewport.display(true);
      */
      break;
    case 4:
      rotary.setUpperBound(0);
      viewport.setCurrentScreen(ABOUT_SCREEN);
      viewport.display(true);
      break;
    default:
      break;
  }

}

void prepScreenClicked()
{
  switch (prepScreen.getCurrentIndex())
  {
    case 0:
      displayMainScreen();
      break;
    default:
      break;
  }
}

void ctrlScreenClicked()
{
  switch (ctrlScreen.getCurrentIndex())
  {
    case 0:
      displayMainScreen();
      break;
    default:
      break;
  }
}

void cardScreenClicked()
{
  switch (cardScreen.getCurrentIndex())
  {
    case 0:
      displayMainScreen();
      break;
    default:
      break;
  }
}

void aboutScreenClicked()
{
  displayMainScreen();
}

void onClicked()
{
  switch (viewport.getCurrentIndex())
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

void onRotationChange(long pos, long prevPos)
{
  switch (viewport.getCurrentIndex())
  {
    case MAIN_SCREEN:
      mainScreen.setFocusPosition(0, (pos - 1) % LCD_ROWS);
      viewport.display();
      break;
    case PREP_SCREEN:
      prepScreen.setFocusPosition(0, (pos - 1) % LCD_ROWS);
      viewport.display();
      break;
    default:
      break;
  }
}

void onRotationCCW()
{
  switch (viewport.getCurrentIndex())
  {
    case MAIN_SCREEN:
      mainScreen.previousLine();
      break;
    case PREP_SCREEN:
      prepScreen.previousLine();
      break;
    case CTRL_SCREEN:
      ctrlScreen.previousLine();
      break;
    case CARD_SCREEN:
      // cardScreen.previousLine();
      break;
    case ABOUT_SCREEN:
      aboutScreen.previousLine();
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
    case PREP_SCREEN:
      prepScreen.nextLine();
      break;
    case CTRL_SCREEN:
      ctrlScreen.nextLine();
      break;
    case CARD_SCREEN:
      // cardScreen.nextLine();
      break;
    case ABOUT_SCREEN:
      aboutScreen.nextLine();
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
  rotary.setLowerBound(0);
  rotary.setUpperBound(MAIN_OPTIONS - 1);

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
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

#define OPTIONS 8;
#define EN_DEBOUNCE 10
#define EN_LI 0
#define EN_LS 7

#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

#define INFO_SCREEN 0
#define MAIN_SCREEN 1

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

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

LiquidViewport viewport(lcd, LCD_COLS, LCD_ROWS);

LiquidScreen infoScreen;
LiquidMenu mainScreen;

LiquidLine info1(0, 0, "Hello World!");

LiquidLine option1(ARROW_OFFSET, 0, "Option 1");
LiquidLine option2(ARROW_OFFSET, 0, "Option 2");
LiquidLine option3(ARROW_OFFSET, 0, "Option 3");
LiquidLine option4(ARROW_OFFSET, 0, "Option 4");
LiquidLine option5(ARROW_OFFSET, 0, "Option 5");
LiquidLine option6(ARROW_OFFSET, 0, "Option 6");
LiquidLine option7(ARROW_OFFSET, 0, "Option 7");
LiquidLine option8(ARROW_OFFSET, 0, "Option 8");

void onInfoClicked() {
  viewport.setCurrentScreen(MAIN_SCREEN);
  viewport.draw();
}

void onMainClicked() {
  switch (mainScreen.getCurrentIndex())
  {
    case 0:
      Serial.println("Clicked option one!");
      break;
    case 1:
      Serial.println("Clicked option two!");
      break;
    case 2:
      Serial.println("Clicked option three!");
      break;
    case 3:
      Serial.println("Clicked option four!");
      break;
    case 4:
      Serial.println("Clicked option five!");
      break;
    case 5:
      Serial.println("Clicked option six!");
      break;
    case 6:
      Serial.println("Clicked option seven!");
      break;
    case 7:
      Serial.println("Clicked option eight!");
      break;
    default:
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
      mainScreen.setFocusPosition(0, pos % LCD_ROWS);
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

  rotary.setDebounceTime(EN_DEBOUNCE);
  rotary.setLowerBound(EN_LI);
  rotary.setUpperBound(EN_LS);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.createChar(ARROW_CHAR, ARROW);

  infoScreen.addLine(0, info1);

  mainScreen.addLine(0, option1);
  mainScreen.addLine(1, option2);
  mainScreen.addLine(2, option3);
  mainScreen.addLine(3, option4);
  mainScreen.addLine(4, option5);
  mainScreen.addLine(5, option6);
  mainScreen.addLine(6, option7);
  mainScreen.addLine(7, option8);

  viewport.addScreen(INFO_SCREEN, infoScreen);
  viewport.addScreen(MAIN_SCREEN, mainScreen);

  viewport.setCurrentScreen(INFO_SCREEN);
  viewport.draw();
}

void loop()
{
  rotary.tick();
}
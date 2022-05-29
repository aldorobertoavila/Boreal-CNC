#include <LiquidCrystal_I2C.h>
#include <Wire.h>

const int LCD_ADDR = 0x27;
const int LCD_COLS = 20;
const int LCD_ROWS = 4;

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void setup()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);

    lcd.print("Hello world!");
}

void loop()
{
    delay(1);
}
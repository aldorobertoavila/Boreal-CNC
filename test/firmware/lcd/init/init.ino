#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

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
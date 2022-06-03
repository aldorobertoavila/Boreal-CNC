#include <util/atomic.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define EN_DT_PIN 2
#define EN_SW_PIN 3
#define EN_CLK_PIN 4

#define EN_DT_PIN 2
#define EN_SW_PIN 3
#define EN_CLK_PIN 4

#define OPTIONS 8;
#define EN_DEBOUNCE 5
#define EN_LI 0
#define EN_LS 7

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

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

LiquidScreen screen;

LiquidLine option1(2, 0, "Option 1");
LiquidLine option2(2, 0, "Option 2");
LiquidLine option3(2, 0, "Option 3");
LiquidLine option4(2, 0, "Option 4");
LiquidLine option5(2, 0, "Option 5");
LiquidLine option6(2, 0, "Option 6");
LiquidLine option7(2, 0, "Option 7");
LiquidLine option8(2, 0, "Option 8");

unsigned long pos;
unsigned long prevPos;

void tick()
{
    rotary.tick();
}

void setup()
{
    Serial.begin(115200);

    pinMode(EN_CLK_PIN, INPUT);
    pinMode(EN_DT_PIN, INPUT);
    pinMode(EN_SW_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(EN_DT_PIN), tick, LOW);

    rotary.setDebounceTime(EN_DEBOUNCE);
    rotary.setLowerBound(EN_LI);
    rotary.setUpperBound(EN_LS);

    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.createChar(ARROW_CHAR, ARROW);
    
    screen.addLine(0, option1);
    screen.addLine(1, option2);
    screen.addLine(2, option3);
    screen.addLine(3, option4);
    screen.addLine(4, option5);
    screen.addLine(5, option6);
    screen.addLine(6, option7);
    screen.addLine(7, option8);

    screen.print(lcd, LCD_COLS, LCD_ROWS);
}

void loop()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        pos = rotary.getPosition();
    }

    if (pos != prevPos)
    {
        if(pos > prevPos) 
        {
            screen.nextLine();
        }
        else if(pos < prevPos)
        {
            screen.previousLine();
        }

        screen.setFocusPosition(0, pos % LCD_ROWS);
        screen.print(lcd, LCD_COLS, LCD_ROWS);
        prevPos = pos;
    }
}
#include <util/atomic.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define EN_DT_PIN 2
#define EN_SW_PIN 3
#define EN_CLK_PIN 4

const byte EN_DEBOUNCE = 5;
const byte EN_LI = 0;
const byte EN_LS = 8;

const byte LCD_ADDR = 0x27;
const byte LCD_COLS = 20;
const byte LCD_ROWS = 4;

const byte ARROW_CHAR = 0;
const byte ARROW_OFFSET = 2;

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

char *MENU[EN_LS] = {"Option 1", "Option 2", "Option 3", "Option 4", "Option 5", "Option 6", "Option 7", "Option 8"};

volatile int volatileCursor;
volatile int volatilePos;
int prevCursor;
int prevPos;

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void encoder()
{
    static unsigned long prevMillis = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - prevMillis > EN_DEBOUNCE) 
    {
        if (digitalRead(EN_CLK_PIN))
        {
            volatilePos++;
        }
        else
        {
            volatilePos--;
        }

        volatilePos = min(EN_LS - 1, max(EN_LI, volatilePos));
        volatileCursor = volatilePos % LCD_ROWS;
        prevMillis = currentMillis;
    }
}

void clearChar(int col, int row)
{
    lcd.setCursor(col, row);
    lcd.print(" ");
}

void printChar(int col, int row, int character)
{
    lcd.setCursor(0, row);
    lcd.write(character);
}

void printMenu(int pos, int prevPos, int cursor, int prevCursor)
{
    if(pos + LCD_ROWS > EN_LS)
    {
        clearChar(0, prevCursor);
        printChar(0, cursor, ARROW_CHAR);
        return;
    }

    lcd.clear();
    printChar(0, 0, ARROW_CHAR);

    for(int i = 0; i < LCD_ROWS; i++) 
    {
        lcd.setCursor(ARROW_OFFSET, i);
        lcd.print(MENU[pos++]);
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(EN_CLK_PIN, INPUT);
    pinMode(EN_DT_PIN, INPUT);
    pinMode(EN_SW_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(EN_DT_PIN), encoder, LOW);

    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.createChar(0, ARROW);
    printMenu(0, 0, 0, 0);
}

void loop()
{
    int cursor;
    int pos;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        cursor = volatileCursor;
        pos = volatilePos;
    }

    if (pos != prevPos)
    {
        Serial.print("Pos: ");
        Serial.print(pos);
        Serial.print(", Cursor: ");
        Serial.println(cursor);
        printMenu(pos, prevPos, cursor, prevCursor);
        prevCursor = cursor;
        prevPos = pos;
    }
}
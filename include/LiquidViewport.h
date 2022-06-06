#include <LiquidCrystal_I2C.h>
#include <LiquidScreen.h>

#define MAX_SCREENS 8

class LiquidViewport
{
public:
    LiquidViewport(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows);

    void addScreen(uint8_t id, LiquidScreen &screen);

    uint8_t getCurrentIndex();

    LiquidScreen *getCurrentScreen();

    void draw();

    void setCurrentScreen(uint8_t id);

protected:
    LiquidCrystal_I2C &_lcd;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _currentScreen;
    uint8_t _screenCount;
    LiquidScreen *_screens[MAX_SCREENS];
};
#include <LiquidCrystal_I2C.h>
#include <LiquidScreen.h>

#define MAX_SCREENS 16

class LiquidViewport
{
public:
    LiquidViewport(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows);

    void addScreen(uint8_t id, LiquidScreen &screen);

    LiquidScreen *getCurrentScreen();

    uint8_t getCurrentLineIndex(uint8_t screenId);

    uint8_t getCurrentScreenIndex();

    LiquidScreen *getScreen(uint8_t screenId);

    void nextLine();

    void previousLine();

    void display();

    void setCurrentScreen(uint8_t id);

protected:
    LiquidCrystal_I2C &_lcd;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _currentScreen;
    uint8_t _screenCount;
    LiquidScreen *_screens[MAX_SCREENS];
};
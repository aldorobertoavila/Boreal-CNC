#include <LiquidCrystal_I2C.h>
#include <LiquidScreen.h>

#ifndef MAX_SCREENS
#define MAX_SCREENS 20
#endif

class LiquidViewport
{
public:
    LiquidViewport(LCD &lcd, uint8_t cols, uint8_t rows);

    void addScreen(uint8_t id, LiquidScreen &screen);

    LiquidScreen *getCurrentScreen();

    uint8_t getCurrentLineIndex(uint8_t screenId);

    uint8_t getCurrentScreenIndex();

    LiquidScreen *getScreen(uint8_t screenId);

    void nextLine();

    void previousLine();

    void display(bool redraw);

    void setCurrentScreen(uint8_t id);

protected:
    LCD &_lcd;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _currentScreen;
    uint8_t _screenCount;
    LiquidScreen *_screens[MAX_SCREENS];
};
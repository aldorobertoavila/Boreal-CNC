#include <LiquidViewport.h>

LiquidViewport::LiquidViewport(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows) : _lcd(lcd), _cols(cols), _rows(rows)
{
    _lcd = lcd;
    _cols = cols;
    _rows = rows;
}

void LiquidViewport::addScreen(uint8_t id, LiquidScreen &screen)
{
    if (_screenCount < MAX_SCREENS)
    {
        _screens[id] = &screen;
        _screenCount++;
    }
}

uint8_t LiquidViewport::getCurrentIndex()
{
    return _currentScreen;
}

LiquidScreen *LiquidViewport::getCurrentScreen()
{
    return _screens[_currentScreen];
}

void LiquidViewport::display(bool redraw)
{
    LiquidScreen *screen = getCurrentScreen();

    if (screen)
    {
        screen->display(_lcd, _cols, _rows, redraw);
    }
}

void LiquidViewport::display()
{
    display(false);
}

void LiquidViewport::setCurrentScreen(uint8_t id)
{
    if (id < MAX_SCREENS)
    {
        _lcd.clear();
        _currentScreen = id;
    }
}
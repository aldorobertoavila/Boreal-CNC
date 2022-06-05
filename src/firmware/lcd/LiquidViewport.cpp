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

LiquidScreen *LiquidViewport::getCurrentScreen() const
{
    return _screens[_currentScreen];
}

void LiquidViewport::nextScreen()
{
    if (_currentScreen < MAX_SCREENS)
    {
        _currentScreen++;
    }
    else
    {
        _currentScreen = 0;
    }
}

void LiquidViewport::previousScreen()
{
    if (_currentScreen > 0)
    {
        _currentScreen--;
    }
    else
    {
        _currentScreen = 0;
    }
}

void LiquidViewport::draw()
{
    LiquidScreen *screen = getCurrentScreen();

    if (screen)
    {
        screen->draw(_lcd, _cols, _rows);
    }
}

void LiquidViewport::setCurrentScreen(uint8_t id)
{
    if (id < MAX_SCREENS)
    {
        _currentScreen = id;
    }
}

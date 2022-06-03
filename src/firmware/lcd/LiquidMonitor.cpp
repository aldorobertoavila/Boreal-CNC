#include <LiquidMonitor.h>

LiquidMonitor::LiquidMonitor(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows) : _lcd(lcd), _cols(cols), _rows(rows)
{
    _lcd = lcd;
    _cols = cols;
    _rows = rows;
}

void LiquidMonitor::addScreen(uint8_t id, LiquidScreen &screen)
{
    if (_screenCount < MAX_SCREENS)
    {
        _screens[id] = &screen;
        _screenCount++;
    }
}

LiquidScreen *LiquidMonitor::getCurrentScreen() const
{
    if (_screenCount > 0)
    {
        return _screens[_currentScreen];
    }

    return nullptr;
}

void LiquidMonitor::nextScreen()
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

void LiquidMonitor::previousScreen()
{
    if (_currentScreen > 0)
    {
        _currentScreen--;
    }
    else
    {
        _currentScreen = _screenCount - 1;
    }
}

void LiquidMonitor::print()
{
    LiquidScreen *screen = getCurrentScreen();

    if (screen)
    {
        screen->print(_lcd, _cols, _rows);
    }
}

void LiquidMonitor::setCurrentScreen(uint8_t id)
{
    if (id < MAX_SCREENS)
    {
        _currentScreen = id;
    }
}

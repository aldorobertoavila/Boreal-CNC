#include <LiquidMonitor.h>

LiquidMonitor::LiquidMonitor(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows, uint8_t startingScreen = 0, uint8_t maxScreens = 8) : _lcd(lcd), _cols(cols), _rows(rows), _currentScreen(startingScreen), _maxScreens(maxScreens)
{
    _lcd = lcd;
    _cols = cols;
    _rows = rows;
    _currentScreen = startingScreen;
    _maxScreens = maxScreens;
}

void LiquidMonitor::addScreen(uint8_t id, LiquidScreen &screen)
{
    if (_screenCount < _maxScreens)
    {
        _screens[id] = screen;
        _screenCount++;
    }
}

LiquidScreen *LiquidMonitor::getCurrentScreen() const
{
    if (_screenCount > 0)
    {
        return &_screens[_currentScreen];
    }
}

void LiquidMonitor::nextScreen()
{
    if (_currentScreen < _maxScreens)
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

bool LiquidMonitor::setCurrentScreen(uint8_t id)
{
    if (id < _maxScreens)
    {
        _currentScreen = id;
    }
}

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
        screen.setCols(_cols);
        screen.setRows(_rows);
        _screens[id] = &screen;
        _screenCount++;
    }
}

LiquidScreen *LiquidViewport::getCurrentScreen()
{
    return getScreen(_currentScreen);
}

uint8_t LiquidViewport::getCurrentLineIndex(uint8_t screenId)
{
    LiquidScreen *screen = getScreen(screenId);

    if (screen)
    {
        return screen->getCurrentLineIndex();
    }

    return 0;
}

uint8_t LiquidViewport::getCurrentScreenIndex()
{
    return _currentScreen;
}

LiquidScreen *LiquidViewport::getScreen(uint8_t screenId)
{
    return _screens[screenId];
}

void LiquidViewport::nextLine()
{
    LiquidScreen *screen = getCurrentScreen();

    if (screen)
    {
        screen->nextLine();
    }
}

void LiquidViewport::previousLine()
{
    LiquidScreen *screen = getCurrentScreen();

    if (screen)
    {
        screen->previousLine();
    }
}

void LiquidViewport::display()
{
    LiquidScreen *screen = getCurrentScreen();

    if (screen)
    {
        screen->display(_lcd);
    }
}

void LiquidViewport::setCurrentScreen(uint8_t id)
{
    if (id < _screenCount)
    {
        _lcd.clear();
        _currentScreen = id;
    }
}
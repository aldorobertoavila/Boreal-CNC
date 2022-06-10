#include <LiquidViewport.h>

LiquidScreen::LiquidScreen()
{
}

void LiquidScreen::addLine(uint8_t id, LiquidLine &line)
{
    if (_lineCount < MAX_LINES)
    {
        _lines[id] = &line;
        _lineCount++;
    }
}

LiquidLine *LiquidScreen::getCurrentLine()
{
    return _lines[_currentLine];
}

uint8_t LiquidScreen::getCurrentLineIndex()
{
    return _currentLine;
}

uint8_t LiquidScreen::getLineCount()
{
    return _lineCount;
}

void LiquidScreen::nextLine()
{
    if (_currentLine < _lineCount - 1)
    {
        setCurrentLine(_currentLine + 1);
    }
    else
    {
        setCurrentLine(0);
    }
}

void LiquidScreen::previousLine()
{
    if (_currentLine > 0)
    {
        setCurrentLine(_currentLine - 1);
    }
    else
    {
        setCurrentLine(0);
    }
}

void LiquidScreen::display(LCD &lcd, bool redraw)
{
    if (redraw)
    {
        displayLines(lcd, _currentLine);
    }
}

void LiquidScreen::displayLines(LCD &lcd, uint8_t startLine)
{
    uint8_t lineIndex = startLine;

    for (int row = 0; row < _rows; row++)
    {
        LiquidLine *line = _lines[lineIndex];

        if (line)
        {
            line->display(lcd);
        }

        lineIndex++;
    }
}

void LiquidScreen::setCurrentLine(uint8_t id)
{
    _prevLine = _currentLine;
    _currentLine = id;
}

void LiquidScreen::setCols(uint8_t cols)
{
    _cols = cols;
}

void LiquidScreen::setRows(uint8_t rows)
{
    _rows = rows;
}

LiquidMenu::LiquidMenu()
{
}

void LiquidMenu::display(LCD &lcd, bool redraw)
{
    if (_lineCount > _rows)
    {
        if (_prevLine > 0)
        {
            lcd.setCursor(0, _prevLine - 1);
            lcd.print(" ");
        }

        if (_currentLine + _rows > _lineCount && _lineCount > 1)
        {
            lcd.setCursor(0, _currentLine - 1);
            lcd.write(_symbol);

            if (redraw)
            {
                displayLines(lcd, _lineCount - _rows);
            }
            return;
        }

        lcd.setCursor(0, 0);
        lcd.write(_symbol);

        displayLines(lcd, _currentLine);
    }
    else
    {
        lcd.setCursor(0, _prevLine);
        lcd.print(" ");

        lcd.setCursor(0, _currentLine);
        lcd.write(_symbol);

        if (redraw)
        {
            displayLines(lcd, 0);
        }
    }
}

void LiquidMenu::displayLines(LCD &lcd, uint8_t startLine)
{
    uint8_t lineIndex = startLine;

    for (int row = 0; row < _rows; row++)
    {
        LiquidLine *line = _lines[lineIndex];

        if (line)
        {
            line->setRow(row);
            line->display(lcd);
            lineIndex++;
        }
    }
}

void LiquidMenu::setFocusSymbol(uint8_t id)
{
    _symbol = id;
}
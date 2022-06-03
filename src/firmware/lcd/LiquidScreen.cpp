#include <LiquidMonitor.h>

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

void LiquidScreen::nextLine()
{
    if (_currentLine < MAX_LINES)
    {
        _currentLine++;
    }
    else
    {
        _currentLine = 0;
    }
}

void LiquidScreen::previousLine()
{
    if (_currentLine > 0)
    {
        _currentLine--;
    }
    else
    {
        _currentLine = 0;
    }
}

void LiquidScreen::print(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows)
{
    lcd.setCursor(0, _prevFocusedRow);
    lcd.print(" ");

    if (_currentLine + rows > _lineCount)
    {
        lcd.setCursor(0, _focusedRow);
        lcd.write(_symbol);
        return;
    }

    lcd.setCursor(0, 0);
    lcd.write(_symbol);

    uint8_t lineIndex = _currentLine;

    for (int row = 0; row < rows; row++)
    {
        LiquidLine *line = _lines[lineIndex];

        if (line)
        {
            line->setRow(row);
            line->print(lcd);
            lineIndex++;
        }
    }
}

void LiquidScreen::setFocusPosition(uint8_t col, uint8_t row)
{
    _prevFocusedCol = _focusedCol;
    _prevFocusedRow = _focusedRow;
    _focusedCol = col;
    _focusedRow = row;
}
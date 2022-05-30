#include <LiquidMonitor.h>

LiquidScreen::LiquidScreen(uint8_t startingLine = 0, uint8_t maxLines = 8) : _currentLine(startingLine), _maxLines(maxLines)
{
    _currentLine = startingLine;
    _maxLines = maxLines;
}

void LiquidScreen::addLine(uint8_t id, LiquidLine &line)
{
    if (_lineCount < _maxLines)
    {
        _lines[id] = line;
        _lineCount++;
    }
}

LiquidLine *LiquidScreen::getCurrentLine() const
{
    return &_lines[_currentLine];
}

void LiquidScreen::nextLine()
{
    if (_currentLine < _maxLines)
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
        _currentLine = _lineCount - 1;
    }
}

void LiquidScreen::print(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows)
{

    if (_currentLine + rows > _lineCount)
    {
        lcd.setCursor(0, _prevFocusedRow);
        lcd.print(" ");

        lcd.setCursor(0, _focusedRow);
        lcd.write(_symbol);
        return;
    }

    lcd.setCursor(0, 0);
    lcd.write(_symbol);

    uint8_t pos = _currentLine;

    for (int i = 0; i < rows; i++)
    {
        LiquidLine *line = getCurrentLine();

        if (line)
        {
            line->print(lcd, cols, rows);
            pos++;
        }
    }
}

bool LiquidScreen::setFocusPosition(uint8_t col, uint8_t row)
{
    _prevFocusedCol = _focusedCol;
    _prevFocusedRow = _focusedRow;
    _focusedCol = col;
    _focusedRow = row;
}

bool LiquidScreen::setFocusSymbol(uint8_t id)
{
    _symbol = id;
}

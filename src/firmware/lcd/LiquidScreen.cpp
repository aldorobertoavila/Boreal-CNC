#include <LiquidScreen.h>

LiquidScreen::LiquidScreen(LCD &lcd, uint8_t cols, uint8_t rows) : _lcd(lcd)
{
    this->_cols = cols;
    this->_rows = rows;
}

void LiquidScreen::append(LiquidLinePtr line)
{
    if (_lineCount < MAX_LINES)
    {
        _lines[_lineCount] = line;
        _lineCount++;
    }
}

void LiquidScreen::clearRow(uint8_t row)
{
    _lcd.setCursor(0, row);
    _lcd.print(std::string("", _rows).c_str());
}

void LiquidScreen::display()
{
    _lcd.clear();
    display(true);
}

void LiquidScreen::display(bool clear)
{
    if (clear)
    {
        draw(_currentLine);
    }
}

void LiquidScreen::draw(uint8_t startLine)
{
    uint8_t lineIndex = _currentLine;

    for (int row = 0; row < _rows; row++)
    {

        if (lineIndex > _lineCount - 1)
        {
            return;
        }

        LiquidLinePtr &ptr = _lines.at(lineIndex);
        LiquidLine *line = ptr.get();

        if (line)
        {
            line->display(_lcd);
        }

        lineIndex++;
    }
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
        setCurrentLine(_currentLine);
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

void LiquidScreen::setCurrentLine(uint8_t id)
{
    _prevLine = _currentLine;
    _currentLine = id;
}

LiquidMenu::LiquidMenu(LCD &lcd, uint8_t cols, uint8_t rows) : LiquidScreen(lcd, cols, rows)
{
}

void LiquidMenu::display()
{
    _lcd.clear();
    display(true);
}

void LiquidMenu::display(bool clear)
{
    uint8_t startLine = _lineCount > _rows ? _lineCount - _rows : 0;

    _lcd.setCursor(0, _prevLine - startLine);
    _lcd.print(" ");

    if (_currentLine + _rows > _lineCount)
    {
        _lcd.setCursor(0, _currentLine - startLine);
        _lcd.write(_symbol);

        if (clear)
        {
            draw(startLine);
        }

        return;
    }

    _lcd.setCursor(0, 0);
    _lcd.write(_symbol);

    draw(_currentLine);
}

void LiquidMenu::draw(uint8_t startLine)
{
    uint8_t lineIndex = startLine;

    for (int row = 0; row < _rows; row++)
    {
        LiquidLinePtr &ptr = _lines.at(lineIndex);
        LiquidLine *line = ptr.get();

        if (line)
        {
            line->setRow(row);
            line->display(_lcd);
            lineIndex++;
        }
    }
}

void LiquidMenu::setFocusSymbol(uint8_t id)
{
    _symbol = id;
}
#include <LiquidScreen.h>

LiquidScreen::LiquidScreen(LCD &lcd, uint8_t cols, uint8_t rows) : _lcd(lcd)
{
    this->_cols = cols;
    this->_rows = rows;

    this->_lineCount = 0;
    this->_currentLine = 0;
    this->_prevLine = 0;
}

void LiquidScreen::append(LiquidLinePtr line)
{
    if (_lineCount < MAX_LINES)
    {
        _lines[_lineCount] = line;
        _lineCount++;
    }
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
    for (uint8_t i = _currentLine; i < _lineCount; i++)
    {
        LiquidLinePtr &line = _lines[i];

        if (line && !line->isHidden())
        {
            line->display(_lcd);
        }
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
    uint8_t lineIndex = _currentLine;

    while (lineIndex < _lineCount - 1)
    {
        lineIndex++;

        LiquidLinePtr line = _lines[lineIndex];

        if (line && !line->isHidden())
        {
            setCurrentLine(lineIndex);
            return;
        }
    }

    setCurrentLine(_currentLine);
}

void LiquidScreen::previousLine()
{
    uint8_t lineIndex = _currentLine;

    while (lineIndex > 0)
    {
        lineIndex--;

        LiquidLinePtr line = _lines[lineIndex];

        if (line && !line->isHidden())
        {
            setCurrentLine(lineIndex);
            return;
        }
    }

    setCurrentLine(_currentLine);
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
    uint8_t hiddenCount = 0;

    for (uint8_t i = 0; i < _lineCount; i++)
    {
        LiquidLinePtr line = _lines[i];

        if (line && line->isHidden())
        {
            hiddenCount++;
        }
    }

    uint8_t visibleCount = _lineCount - hiddenCount;
    uint8_t startLine = visibleCount > _rows ? visibleCount - _rows : 0;

    _lcd.setCursor(0, _lines[_prevLine]->getRow());
    _lcd.print(" ");

    if (_currentLine + _rows - hiddenCount > visibleCount)
    {
        if (clear)
        {
            draw(startLine + hiddenCount);
        }

        _lcd.setCursor(0, _lines[_currentLine]->getRow());
        _lcd.write(_symbol);

        return;
    }

    _lcd.setCursor(0, 0);
    _lcd.write(_symbol);

    draw(_currentLine);
}

void LiquidMenu::draw(uint8_t startLine)
{
    uint8_t lineIndex = startLine;
    uint8_t row = 0;

    while (row < _rows)
    {
        if (lineIndex > _lineCount - 1)
        {
            return;
        }

        LiquidLinePtr line = _lines[lineIndex];

        if (line && !line->isHidden())
        {
            line->setRow(row);
            line->display(_lcd);
            row++;
        }

        lineIndex++;
    }
}

void LiquidMenu::setFocusSymbol(uint8_t id)
{
    _symbol = id;
}

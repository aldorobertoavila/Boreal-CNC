#include <LiquidScreen.h>

LiquidLine::LiquidLine(uint8_t col, uint8_t row, String text)
{
    this->_col = col;
    this->_row = row;
    this->_text = text;
    this->_hidden = false;
}

uint8_t LiquidLine::getColumn()
{
    return _col;
}

uint8_t LiquidLine::getRow()
{
    return _row;
}

String LiquidLine::getText()
{
    return _text;
}

void LiquidLine::hide()
{
    _hidden = true;
}

bool LiquidLine::isHidden()
{
    return _hidden;
}

void LiquidLine::unhide()
{
    _hidden = false;
}

void LiquidLine::display(LCD &lcd)
{
    if (!_text.isEmpty())
    {
        lcd.setCursor(_col, _row);
        lcd.print(_text);
    }
}

void LiquidLine::setColumn(uint8_t col)
{
    _col = col;
}

void LiquidLine::setRow(uint8_t row)
{
    _row = row;
}

void LiquidLine::setText(String text)
{
    _text = text;
}

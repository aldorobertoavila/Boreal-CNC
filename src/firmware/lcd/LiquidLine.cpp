#include <LiquidScreen.h>

LiquidLine::LiquidLine(uint8_t col, uint8_t row, const char *text)
{
    this->_col = col;
    this->_row = row;
    this->_text = text;

    this->_hidden = false;
    this->_symbol = 0;
}

LiquidLine::LiquidLine()
{
    this->_col = 0;
    this->_row = 0;
    this->_text = "";
    this->_hidden = false;
    this->_symbol = 0;
}

uint8_t LiquidLine::getColumn()
{
    return _col;
}

uint8_t LiquidLine::getRow()
{
    return _row;
}

const char *LiquidLine::getText()
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

void LiquidLine::displayText(LCD &lcd)
{
    lcd.setCursor(_col, _row);
    lcd.print(_text);
}

void LiquidLine::displaySymbol(LCD &lcd)
{
    lcd.setCursor(_col, _row);
    lcd.write(_symbol);
}

void LiquidLine::setColumn(uint8_t col)
{
    _col = col;
}

void LiquidLine::setRow(uint8_t row)
{
    _row = row;
}

void LiquidLine::setText(const char *text)
{
    _text = text;
}

void LiquidLine::setSymbol(uint8_t symbol)
{
    _symbol = symbol;
}
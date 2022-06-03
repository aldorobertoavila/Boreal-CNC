#include <LiquidMonitor.h>

LiquidLine::LiquidLine(uint8_t col, uint8_t row, char *text) : _col(col), _row(row), _text(text)
{
    _col = col;
    _row = row;
    _text = text;
}

uint8_t LiquidLine::getColumn() const
{
    return _col;
}

uint8_t LiquidLine::getRow() const
{
    return _row;
}

char *LiquidLine::getText()
{
    return _text;
}

void LiquidLine::print(LiquidCrystal_I2C &lcd)
{
    lcd.setCursor(_col, _row);
    lcd.print(_text);
}

void LiquidLine::setColumn(uint8_t col)
{
    _col = col;
}

void LiquidLine::setRow(uint8_t row)
{
    _row = row;
}

void LiquidLine::setText(char *text)
{
    _text = text;
}
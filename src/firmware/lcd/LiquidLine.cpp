#include <LiquidMonitor.h>

LiquidLine::LiquidLine(uint8_t col, uint8_t row, String text) : _col(col), _row(row), _text(text)
{
    _col = col;
    _row = row;
    _text = text;
}

void LiquidLine::attach(uint8_t id, voidFunc callback)
{
    _functions[id] = &callback;
}

void LiquidLine::call(uint8_t id)
{
    voidFunc func = (voidFunc) _functions[id];

    if (func)
    {
        func();
    }
}

uint8_t LiquidLine::getColumn() const
{
    return _col;
}

uint8_t LiquidLine::getRow() const
{
    return _row;
}

String LiquidLine::getText()
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

void LiquidLine::setText(String text)
{
    _text = text;
}
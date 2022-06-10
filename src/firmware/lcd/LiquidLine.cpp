#include <LiquidViewport.h>

LiquidLine::LiquidLine(uint8_t col, uint8_t row, char *text) : _col(col), _row(row), _text(text)
{
    _col = col;
    _row = row;
    _text = text;
}

uint8_t LiquidLine::getColumn()
{
    return _col;
}

void LiquidLine::setFormat(FormatFunc format)
{
    _format = format;
}

uint8_t LiquidLine::getRow()
{
    return _row;
}

char *LiquidLine::getText()
{
    return _text;
}

void LiquidLine::display(LCD &lcd)
{
    lcd.setCursor(_col, _row);

    if (_format)
    {
        char bytearray[BUFFER_SIZE];
        Buffer buf = bytearray;
        _format(buf, _text);
        lcd.print(buf);
    }
    else
    {
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

void LiquidLine::setText(char *text)
{
    _text = text;
}
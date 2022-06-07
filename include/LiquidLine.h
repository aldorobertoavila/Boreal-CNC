#include <LiquidCrystal_I2C.h>

class LiquidLine
{
public:
    LiquidLine(uint8_t col, uint8_t row, String text);

    uint8_t getColumn();

    uint8_t getRow();

    String getText();

    void display(LiquidCrystal_I2C &lcd);

    uint8_t length();

    void setColumn(uint8_t col);

    void setRow(uint8_t row);

    void setText(String text);

protected:
    uint8_t _col;
    uint8_t _row;
    String _text;
};
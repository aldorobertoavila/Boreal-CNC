#include <LiquidCrystal_I2C.h>

class LiquidLine
{
public:
    LiquidLine(uint8_t col, uint8_t row, String text);

    uint8_t getColumn() const;

    uint8_t getRow() const;

    String getText();

    void draw(LiquidCrystal_I2C &lcd);

    void setColumn(uint8_t col);

    void setRow(uint8_t row);

    void setText(String text);

protected:
    uint8_t _col;
    uint8_t _row;
    String _text;
};
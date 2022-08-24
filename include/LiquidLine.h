#include <LiquidCrystal_I2C.h>

using LCD = LiquidCrystal_I2C;
class LiquidLine
{
public:
    LiquidLine(uint8_t col, uint8_t row, String text);

    uint8_t getColumn();

    uint8_t getRow();

    String getText();

    void hide();

    bool isHidden();

    void unhide();

    void display(LCD &lcd);

    void setColumn(uint8_t col);

    void setRow(uint8_t row);

    void setText(String text);

protected:
    uint8_t _col;
    uint8_t _row;
    bool _hidden;
    String _text;
};
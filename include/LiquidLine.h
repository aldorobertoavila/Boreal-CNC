#include <LiquidCrystal_I2C.h>

#define MAX_FUNCTIONS 4

typedef void (*voidFunc)();
class LiquidLine
{
public:
    LiquidLine(uint8_t col, uint8_t row, String text);

    void attach(uint8_t id, voidFunc callback);

    void call(uint8_t id);

    uint8_t getColumn() const;

    uint8_t getRow() const;

    String getText();

    void print(LiquidCrystal_I2C &lcd);

    void setColumn(uint8_t col);

    void setRow(uint8_t row);

    void setText(String text);

protected:
    voidFunc *_functions[MAX_FUNCTIONS];
    uint8_t _col;
    uint8_t _row;
    String _text;
};
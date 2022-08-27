#include <hd44780ioClass/hd44780_I2Cexp.h>

using LCD = hd44780_I2Cexp;

class LiquidLine
{
public:
    LiquidLine(uint8_t col, uint8_t row, const char *text);

    uint8_t getColumn();

    uint8_t getRow();

    const char *getText();

    void hide();

    bool isHidden();

    void unhide();

    void displayText(LCD &lcd);

    void displaySymbol(LCD &lcd);

    void setColumn(uint8_t col);

    void setRow(uint8_t row);

    void setText(const char *text);

    void setSymbol(uint8_t symbol);

protected:
    uint8_t _col;
    uint8_t _row;
    uint8_t _symbol;
    bool _hidden;
    const char *_text;
};
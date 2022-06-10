#include <LiquidCrystal_I2C.h>

#define BUFFER_SIZE 20

using LCD = LiquidCrystal_I2C;

typedef char (&Buffer)[BUFFER_SIZE];
typedef void (*FormatFunc)(Buffer, char *);

class LiquidLine
{
public:
    LiquidLine(uint8_t col, uint8_t row, char *text);

    uint8_t getColumn();

    uint8_t getRow();

    char *getText();

    virtual void display(LCD &lcd);

    void setColumn(uint8_t col);

    void setFormat(FormatFunc format);

    void setRow(uint8_t row);

    void setText(char *text);

protected:
    uint8_t _col;
    uint8_t _row;
    char *_text;
    FormatFunc _format;
};
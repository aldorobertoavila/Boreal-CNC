#include <LiquidCrystal_I2C.h>

#include <Printable.h>

#define BUFFER_SIZE 20

using LCD = LiquidCrystal_I2C;

typedef char (&Buffer)[BUFFER_SIZE];
typedef void (*FormatFunc)(Buffer, char *);

class LiquidLine
{
public:
    LiquidLine(uint8_t col, uint8_t row, String text);

    uint8_t getColumn();

    uint8_t getRow();

    String getText();

    virtual void display(LCD &lcd);

    void setColumn(uint8_t col);

    void setRow(uint8_t row);

    void setText(String text);

protected:
    uint8_t _col;
    uint8_t _row;
    String _text;
};

class LiquidFormattedLine : public LiquidLine
{
public:
    LiquidFormattedLine();

    void display(LCD &lcd) override;

protected:
    FormatFunc _format;
};
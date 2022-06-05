#include <LiquidCrystal_I2C.h>
#include <LiquidLine.h>

#define MAX_LINES 16

class LiquidScreen
{
public:
    LiquidScreen();

    void addLine(uint8_t id, LiquidLine &line);

    uint8_t getCurrentIndex();

    LiquidLine *getCurrentLine();

    void nextLine();

    void previousLine();

    void draw(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows);

    void setCurrentLine(uint8_t id);

    void setFocusPosition(uint8_t col, uint8_t row);

    void setFocusSymbol(uint8_t id);

protected:
    uint8_t _currentLine;
    uint8_t _focusedCol;
    uint8_t _focusedRow;
    uint8_t _focusedLine;
    uint8_t _lineCount;
    LiquidLine *_lines[MAX_LINES];
    uint8_t _prevFocusedCol;
    uint8_t _prevFocusedRow;
    uint8_t _symbol;
};
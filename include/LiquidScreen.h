#include <LiquidCrystal_I2C.h>
#include <LiquidLine.h>

#define MAX_LINES 16

class LiquidScreen
{
public:
    LiquidScreen();

    void addLine(uint8_t id, LiquidLine &line);

    LiquidLine *getCurrentLine();

    uint8_t getCurrentLineIndex();

    uint8_t getLineCount();

    void nextLine();

    void previousLine();

    virtual void display(LiquidCrystal_I2C &lcd);

    void displayLines(LiquidCrystal_I2C &lcd, uint8_t startLine);

    void setCurrentLine(uint8_t id);

    void setCols(uint8_t cols);

    void setRows(uint8_t rows);

protected:
    uint8_t _currentLine;
    uint8_t _prevLine;
    uint8_t _lineCount;
    uint8_t _cols;
    uint8_t _rows;
    LiquidLine *_lines[MAX_LINES];
};

class LiquidMenu : public LiquidScreen
{
public:
    LiquidMenu();

    void display(LiquidCrystal_I2C &lcd) override;

    void setFocusSymbol(uint8_t id);

protected:
    uint8_t _symbol;
};
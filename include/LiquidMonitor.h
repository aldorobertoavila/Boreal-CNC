#include <LiquidCrystal_I2C.h>

#define MAX_SCREENS 8
#define MAX_LINES 16

class LiquidLine
{
public:
    LiquidLine(uint8_t col, uint8_t row, char *text);

    uint8_t getColumn() const;

    uint8_t getRow() const;

    char *getText();

    void print(LiquidCrystal_I2C &lcd);

    void setColumn(uint8_t col);

    void setRow(uint8_t row);

    void setText(char *text);

protected:
    uint8_t _col;
    uint8_t _row;
    char *_text;
};

class LiquidScreen
{
public:
    LiquidScreen();

    void addLine(uint8_t id, LiquidLine &line);

    LiquidLine *getCurrentLine();

    void nextLine();

    void previousLine();

    void print(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows);

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


class LiquidMonitor
{
public:
    LiquidMonitor(LiquidCrystal_I2C &lcd, uint8_t cols, uint8_t rows);

    void addScreen(uint8_t id, LiquidScreen &screen);

    LiquidScreen *getCurrentScreen() const;

    void nextScreen();

    void previousScreen();

    void print();

    void setCurrentScreen(uint8_t id);

protected:
    LiquidCrystal_I2C _lcd;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _currentScreen;
    uint8_t _screenCount;
    LiquidScreen *_screens[MAX_SCREENS];
};
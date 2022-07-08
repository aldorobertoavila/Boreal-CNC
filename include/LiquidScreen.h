#include <LiquidLine.h>

#ifndef MAX_LINES
#define MAX_LINES 10
#endif

class LiquidScreen
{
public:
    LiquidScreen();

    void addLine(LiquidLine &line);

    LiquidLine *getCurrentLine();

    uint8_t getCurrentLineIndex();

    uint8_t getLineCount();

    void nextLine();

    void previousLine();

    virtual void display(LCD &lcd, bool redraw);

    virtual void displayLines(LCD &lcd, uint8_t startLine);

    void setCurrentLine(uint8_t id);

    void setCols(uint8_t cols);

    void setRows(uint8_t rows);

    static void createLine(LiquidScreen &screen, uint8_t col, uint8_t row, char *text);

    static void createFormattedLine(LiquidScreen &screen, uint8_t col, uint8_t row, char *text);

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

    void display(LCD &lcd, bool redraw) override;

    void displayLines(LCD &lcd, uint8_t startLine) override;

    void setFocusSymbol(uint8_t id);

protected:
    uint8_t _symbol;
};
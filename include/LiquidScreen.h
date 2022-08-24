#include <LiquidLine.h>

#include <array>
#include <memory>

#ifndef MAX_LINES
#define MAX_LINES 12
#endif

using namespace std;

using LiquidLinePtr = std::shared_ptr<LiquidLine>;

class LiquidScreen
{
public:
    LiquidScreen(LCD &lcd, uint8_t cols, uint8_t rows);

    void append(uint8_t lineIndex, LiquidLinePtr line);

    virtual void display();

    virtual void display(bool clear);

    virtual void draw(uint8_t startLine);

    uint8_t getCurrentLineIndex();

    uint8_t getLineCount();

    void hide(uint8_t lineIndex);

    void unhide(uint8_t lineIndex);

    void nextLine();

    void previousLine();

    void setCurrentLine(uint8_t id);

protected:
    uint8_t _currentLine;
    uint8_t _prevLine;
    uint8_t _visibleCount;
    uint8_t _lineCount;
    uint8_t _cols;
    uint8_t _rows;
    LCD &_lcd;
    LiquidLinePtr _lines[MAX_LINES];
};

class LiquidMenu : public LiquidScreen
{
public:
    LiquidMenu(LCD &lcd, uint8_t cols, uint8_t rows);

    void display() override;

    void display(bool clear) override;

    void draw(uint8_t startLine) override;

    void setFocusSymbol(uint8_t id);

protected:
    uint8_t _symbol;
};
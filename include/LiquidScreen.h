#include <LiquidLine.h>

#include <array>
#include <memory>

#ifndef MAX_LINES
#define MAX_LINES 12
#endif

using namespace std;

using LiquidLinePtr = std::shared_ptr<LiquidLine>;

class ILiquidScreen
{
public:

    virtual void append(LiquidLinePtr line) = 0;

    virtual void display() = 0;

    virtual void display(bool clear) = 0;

    virtual void draw(uint8_t startLine) = 0;

    virtual uint8_t getCurrentLineIndex() = 0;

    virtual uint8_t getLineCount() = 0;

    virtual void nextLine() = 0;

    virtual void previousLine() = 0;

    virtual void setCurrentLine(uint8_t id) = 0;
};

class LiquidScreen : public ILiquidScreen
{
public:
    LiquidScreen(LCD &lcd, uint8_t cols, uint8_t rows);

    void append(LiquidLinePtr line) override;

    void display() override;

    void display(bool clear) override;
    
    void draw(uint8_t startLine) override;

    uint8_t getCurrentLineIndex() override;

    uint8_t getLineCount() override;

    void nextLine() override;

    void previousLine() override;

    void setCurrentLine(uint8_t id) override;

protected:
    uint8_t _currentLine;
    uint8_t _prevLine;
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
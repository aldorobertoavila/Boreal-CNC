typedef void (*voidFunc)();

class Rotary
{
public:
    Rotary();

    Rotary(uint8_t dt, uint8_t clk, uint8_t sw);

    void forcePosition(long position);

    long getPosition();

    void onClicked(voidFunc onClicked);

    void onRotationCW(voidFunc onRotationCW);

    void onRotationCCW(voidFunc onRotationCCW);

    void setClickDebounceTime(unsigned long debounceTime);

    void setRotationDebounceTime(unsigned long debounceTime);

    void setLowerBound(int lowerBound);

    void setPosition(long position, bool callback);

    void setUpperBound(int upperBound);

    void tick();

private:
    long _position;
    long _prevPosition;
    unsigned long _clickDebounceTime;
    unsigned long _rotationDebounceTime;
    uint8_t _dt;
    uint8_t _clk;
    uint8_t _sw;
    int _lowerBound = INT16_MIN;
    int _upperBound = INT16_MIN;
    voidFunc _onClicked;
    voidFunc _onRotationCW;
    voidFunc _onRotationCCW;
};

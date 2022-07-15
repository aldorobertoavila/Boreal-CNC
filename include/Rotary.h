typedef void (*voidFunc)();

class Rotary
{
public:

    Rotary(uint8_t dt, uint8_t clk, uint8_t sw);

    void forcePosition(long position);

    long getPosition();

    long getPrevPosition();

    void onClicked(voidFunc onClicked);

    void onRotationCW(voidFunc onRotationCW);

    void onRotationCCW(voidFunc onRotationCCW);

    void setBounds(long lowerBound, long upperBound);

    void setDebounceTime(unsigned long clickDebounce, unsigned long rotationDebounce);

    void setPosition(long position, bool callback);

    void tick();

private:
    long _position;
    long _prevPosition;
    unsigned long _clickDebounceTime;
    unsigned long _rotationDebounceTime;
    uint8_t _dt;
    uint8_t _clk;
    uint8_t _sw;
    int _lowerBound;
    int _upperBound;
    voidFunc _onClicked;
    voidFunc _onRotationCW;
    voidFunc _onRotationCCW;
};

typedef void (*changeCallback)(long int, long int);
typedef void (*rotaryCallback)();

class Rotary
{
public:
    Rotary();

    Rotary(uint8_t dt, uint8_t clk, uint8_t sw);

    void tick();
    volatile long getPosition();
    volatile long getPreviousPosition();

    void onClicked(rotaryCallback onClicked);
    void onRotationChange(changeCallback onRotationChange);
    void onRotationCW(rotaryCallback onRotationCW);
    void onRotationCCW(rotaryCallback onRotationCCW);

    void setDebounceTime(unsigned long debounceTime);
    void setLowerBound(int lowerBound);
    void setPosition(long position, bool callback);
    void setPosition(long position);
    void setUpperBound(int upperBound);

private:
    volatile long _position;
    volatile long _prevPosition;
    unsigned long _debounceTime;
    uint8_t _dt;
    uint8_t _clk;
    uint8_t _sw;
    int _lowerBound = INT16_MIN;
    int _upperBound = INT16_MIN;
    changeCallback _onChange;
    rotaryCallback _onClicked;
    rotaryCallback _onRotationCW;
    rotaryCallback _onRotationCCW;
};
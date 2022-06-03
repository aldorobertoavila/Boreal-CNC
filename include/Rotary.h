typedef void (*changeCallback)(long int, long int);
typedef void (*rotationCallback)();

class Rotary
{
public:
    Rotary();

    Rotary(uint8_t dt, uint8_t clk, uint8_t sw);

    void tick();
    volatile long getPosition();
    volatile long getPreviousPosition();

    void onChange(changeCallback onChange);
    void onRotationCW(rotationCallback onRotationCW);
    void onRotationCCW(rotationCallback onRotationCCW);

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
    rotationCallback _onRotationCW;
    rotationCallback _onRotationCCW;
};
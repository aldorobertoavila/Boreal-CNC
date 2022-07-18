#include <Arduino.h>
#include <Rotation.h>

typedef void (*onClicked)();
typedef void (*onRotation)(Rotation);

class Rotary
{
public:

    Rotary(uint8_t clkPin, uint8_t dtPin, uint8_t swPin);

    long getPosition();

    long getPrevPosition();

    void setBoundaries(long lowerBound, long upperBound);

    void setClickDebounceTime(unsigned long debounceTime);

    void setRotationDebounceTime(unsigned long debounceTime);

    void setDefaultDirection(Rotation direction);

    void setPosition(long position);

    void setOnClicked(onClicked callback);

    void setOnRotation(onRotation callback);

    void tick();

private:
    long _position;
    long _prevPosition;
    unsigned long _clickDebounceTime;
    unsigned long _rotationDebounceTime;
    uint8_t _dtPin;
    uint8_t _clkPin;
    uint8_t _swPin;
    int _lowerBound;
    int _upperBound;
    Rotation _defaultDirection;
    onClicked _onClicked;
    onRotation _onRotation;
};
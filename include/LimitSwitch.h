#include <Arduino.h>

class LimitSwitch
{
public:
    LimitSwitch(uint8_t swPin);

    bool isPressed();

    void tick();

private:
    uint8_t _swPin;
    bool _isPressed;
};

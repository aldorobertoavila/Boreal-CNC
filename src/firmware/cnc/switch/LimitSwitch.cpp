#include <LimitSwitch.h>

LimitSwitch::LimitSwitch(uint8_t swPin)
{
    this->_swPin = swPin;
}

bool LimitSwitch::isPressed()
{
    return _isPressed;
}

void LimitSwitch::tick()
{
    _isPressed = digitalRead(_swPin);
}
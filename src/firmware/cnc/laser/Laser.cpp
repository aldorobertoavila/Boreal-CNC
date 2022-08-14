#include <Arduino.h>
#include <Laser.h>

Laser::Laser(uint8_t pwmPin)
{
    this->_pwmPin = pwmPin;
}

InlineMode Laser::getInlineMode()
{
    return _mode;
}

uint8_t Laser::getMaxPower()
{
    return _maxPower;
}

uint8_t Laser::getPower()
{
    return _power;
}

void Laser::setInlineMode(InlineMode mode)
{
    _mode = mode;
}

void Laser::setMaxPower(uint8_t maxPower)
{
    _maxPower = maxPower;
}

void Laser::setPower(uint8_t power)
{
    _power = constrain(power, 0, _maxPower);
}

void Laser::turnOn()
{
    if (_power > 0)
    {
        analogWrite(_pwmPin, _power);
    }
}

void Laser::turnOff()
{
    setPower(0);
    analogWrite(_pwmPin, 0);
}
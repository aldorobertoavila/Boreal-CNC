#include <Arduino.h>
#include <Rotary.h>

Rotary::Rotary(uint8_t dt, uint8_t clk, uint8_t sw) : _dt(dt), _clk(clk), _sw(sw)
{
    _dt = dt;
    _clk = clk;
    _sw = sw;
}

volatile long Rotary::getPosition()
{
    return _position;
}

volatile long Rotary::getPreviousPosition()
{
    return _prevPosition;
}

void Rotary::onChange(changeCallback onChange)
{
    _onChange = onChange;
}

void Rotary::onRotationCW(rotationCallback onRotationCW)
{
    _onRotationCW = onRotationCW;
}

void Rotary::onRotationCCW(rotationCallback onRotationCCW)
{
    _onRotationCCW = onRotationCCW;
}

void Rotary::tick()
{
    static unsigned long prevRotationMillis = 0;
    unsigned long currentMillis = millis();

    if (digitalRead(_dt) == LOW)
    {
        if (currentMillis - prevRotationMillis > _debounceTime)
        {
            if (digitalRead(_clk))
            {
                setPosition(_position + 1);
            }
            else
            {
                setPosition(_position - 1);
            }
        }

        prevRotationMillis = currentMillis;
    }
}

void Rotary::setDebounceTime(unsigned long debounceTime)
{
    _debounceTime = debounceTime;
}

void Rotary::setLowerBound(int lowerBound)
{
    _lowerBound = lowerBound;
}

void Rotary::setPosition(long position, bool callback)
{
    _prevPosition = _position;
    _position = position;

    if (_position < _lowerBound)
    {
        _position = _lowerBound;
    }
    else if (_position > _upperBound)
    {
        _position = _upperBound;
    }

    if (_position != _prevPosition && callback)
    {
        if (_position > _prevPosition && _onRotationCW)
        {
            _onRotationCW();
        }
        else if (_position < _prevPosition && _onRotationCCW)
        {
            _onRotationCCW();
        }

        if (_onChange)
        {
            _onChange(_position, _position);
        }
    }
}

void Rotary::setPosition(long position)
{
    setPosition(position, true);
}

void Rotary::setUpperBound(int upperBound)
{
    _upperBound = upperBound;
}
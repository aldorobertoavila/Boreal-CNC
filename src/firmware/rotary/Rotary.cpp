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
    return _volatilePos;
}

void Rotary::tick()
{
    static unsigned long prevMillis = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - prevMillis > _debounceTime)
    {
        if (digitalRead(_clk))
        {
            _volatilePos++;
        }
        else
        {
            _volatilePos--;
        }

        if (_volatilePos < _lowerBound)
        {
            _volatilePos = _lowerBound;
        }
        else if (_volatilePos > _upperBound)
        {
            _volatilePos = _upperBound;
        }
    }

    prevMillis = currentMillis;
}

void Rotary::setDebounceTime(unsigned long debounceTime)
{
    _debounceTime = debounceTime;
}

void Rotary::setLowerBound(int lowerBound)
{
    _lowerBound = lowerBound;
}

void Rotary::setPosition(long position)
{
    _volatilePos = position;
}

void Rotary::setUpperBound(int upperBound)
{
    _upperBound = upperBound;
}
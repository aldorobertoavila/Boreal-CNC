#include <Arduino.h>
#include <Rotory.h>

Rotory::Rotory(uint8_t dt, uint8_t clk, uint8_t sw) : _dt(dt), _clk(clk), _sw(sw)
{
    _dt = dt;
    _clk = clk;
    _sw = sw;
}

volatile long Rotory::getPosition()
{
    return _volatilePos;
}

void Rotory::tick()
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

void Rotory::setDebounceTime(long debounceTime)
{
    _debounceTime = debounceTime;
}

void Rotory::setLowerBound(int lowerBound)
{
    _lowerBound = lowerBound;
}

void Rotory::setPosition(long position)
{
    _volatilePos = position;
}

void Rotory::setUpperBound(int upperBound)
{
    _upperBound = upperBound;
}
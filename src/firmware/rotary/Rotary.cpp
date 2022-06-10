#include <Arduino.h>
#include <Rotary.h>

Rotary::Rotary(uint8_t dt, uint8_t clk, uint8_t sw) : _dt(dt), _clk(clk), _sw(sw)
{
  _dt = dt;
  _clk = clk;
  _sw = sw;
}

void Rotary::forcePosition(long position)
{
  setPosition(position, false);
}

long Rotary::getPosition()
{
  return _position;
}

void Rotary::onClicked(voidFunc onClicked)
{
  _onClicked = onClicked;
}

void Rotary::onRotationCW(voidFunc onRotationCW)
{
  _onRotationCW = onRotationCW;
}

void Rotary::onRotationCCW(voidFunc onRotationCCW)
{
  _onRotationCCW = onRotationCCW;
}

void Rotary::setClickDebounceTime(unsigned long debounceTime)
{
  _clickDebounceTime = debounceTime;
}

void Rotary::setRotationDebounceTime(unsigned long debounceTime)
{
  _rotationDebounceTime = debounceTime;
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
  else if (_position >= _upperBound)
  {
    _position = _upperBound - 1;
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
  }
}

void Rotary::setUpperBound(int upperBound)
{
  _upperBound = upperBound;
}

void Rotary::tick()
{
  static unsigned long prevClickMillis = 0;
  static unsigned long prevRotationMillis = 0;
  unsigned long currentMillis = millis();

  if (digitalRead(_dt) == LOW)
  {
    if (currentMillis - prevRotationMillis > _rotationDebounceTime)
    {
      if (digitalRead(_clk))
      {
        setPosition(_position + 1, true);
      }
      else
      {
        setPosition(_position - 1, true);
      }
    }

    prevRotationMillis = currentMillis;
  }

  if (digitalRead(_sw) == LOW)
  {
    if (currentMillis - prevClickMillis > _clickDebounceTime)
    {
      if (_onClicked)
      {
        _onClicked();
        prevClickMillis = currentMillis;
      }
    }
  }
}

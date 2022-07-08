#include <Arduino.h>
#include <Rotary.h>

Rotary::Rotary(uint8_t dt, uint8_t clk, uint8_t sw)
{
  this->_dt = dt;
  this->_clk = clk;
  this->_sw = sw;
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

void Rotary::setBounds(long lowerBound, long upperBound)
{
  _lowerBound = lowerBound;
  _upperBound = upperBound;
}

void Rotary::setDebounceTime(unsigned long clickDebounce, unsigned long rotationDebounce)
{
  _clickDebounceTime = clickDebounce;
  _rotationDebounceTime = rotationDebounce;
}

void Rotary::setPosition(long position, bool callback)
{
  _prevPosition = _position;
  _position = constrain(position, _lowerBound, _upperBound);

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
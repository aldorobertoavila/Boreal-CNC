#include <Rotary.h>

Rotary::Rotary(uint8_t clkPin, uint8_t dtPin, uint8_t swPin)
{
  this->_clkPin = clkPin;
  this->_dtPin = dtPin;
  this->_swPin = swPin;

  this->_defaultDirection = Rotation::CLOCKWISE;
  this->_previousStateClk = digitalRead(_clkPin);

  this->_lowerBound = 0;
  this->_upperBound = 1;

  pinMode(_clkPin, INPUT);
  pinMode(_dtPin, INPUT);
  pinMode(_swPin, INPUT_PULLUP);
}

long Rotary::getPrevPosition()
{
  return _prevPosition;
}

long Rotary::getPosition()
{
  return _position;
}

void Rotary::setBoundaries(long lowerBound, long upperBound)
{
  _lowerBound = lowerBound;
  _upperBound = upperBound;
}

void Rotary::setClickDebounceTime(unsigned long debounceTime)
{
  _clickDebounceTime = debounceTime;
}

void Rotary::setDefaultDirection(Rotation direction)
{
  _defaultDirection = direction;
}

void Rotary::setInterval(unsigned long interval)
{
  _interval = interval;
}

void Rotary::setPosition(long position)
{
  _prevPosition = _position;
  _position = constrain(position, _lowerBound, _upperBound);
}

void Rotary::setOnClicked(onClicked callback)
{
  _onClicked = callback;
}

void Rotary::setOnRotation(onRotation callback)
{
  _onRotation = callback;
}

void Rotary::setRotationDebounceTime(unsigned long debounceTime)
{
  _rotationDebounceTime = debounceTime;
}

void Rotary::tick()
{
  unsigned long currentMillis = millis();

  if (currentMillis - _lastRotationTime > _rotationDebounceTime)
  {
    bool currentStateClk = digitalRead(_clkPin);

    if (currentStateClk != _previousStateClk)
    {
      if (digitalRead(_dtPin) != currentStateClk)
      {
        if (_defaultDirection == Rotation::CLOCKWISE)
        {
          setPosition(_position - _interval);
        }
        else
        {
          setPosition(_position + _interval);
        }

        _onRotation(Rotation::COUNTERCLOCKWISE);
      }
      else
      {
        if (_defaultDirection == Rotation::CLOCKWISE)
        {
          setPosition(_position + _interval);
        }
        else
        {
          setPosition(_position - _interval);
        }

        _onRotation(Rotation::CLOCKWISE);
      }
      _previousStateClk = currentStateClk;
      _lastRotationTime = currentMillis;
    }
  }

  if (!digitalRead(_swPin))
  {
    if (currentMillis - _lastClickTime > _clickDebounceTime)
    {
      if (_onClicked)
      {
        _onClicked();
        _lastClickTime = currentMillis;
      }
    }
  }
}
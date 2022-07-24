#include <LimitSwitch.h>

LimitSwitch::LimitSwitch(uint8_t swPin) {
  this->_swPin = swPin;
  this->_previousState = digitalRead(swPin);
}

bool LimitSwitch::isPressed() {
  return _previousState && !_currentState;
}

void LimitSwitch::tick() {
  static unsigned long lastDebounceTime;
  unsigned long currentTime = millis();

  if(currentTime - lastDebounceTime > _debounceTime)
  {
    _previousState = _currentState;
    _currentState = digitalRead(_swPin);
    lastDebounceTime = currentTime;
  }
}

bool LimitSwitch::wasPressed() {
  static bool alreadyRead = HIGH;
  bool wasPressed = LOW;
 
  if (!_currentState && !alreadyRead)
  {
    wasPressed = HIGH;
  }

  alreadyRead = !_currentState;

  return wasPressed;
}

bool LimitSwitch::wasReleased() {
  static bool alreadyRead = HIGH;
  bool wasReleased = LOW;
 
  if (_currentState && !alreadyRead)
  {
    wasReleased = HIGH;
  }

  alreadyRead = _currentState;

  return wasReleased;
}

void LimitSwitch::setDebounceTime(unsigned long debounceTime)
{
  _debounceTime = debounceTime;
}

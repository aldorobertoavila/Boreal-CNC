#include <LimitSwitch.h>

LimitSwitch::LimitSwitch(uint8_t swPin) {
  this->_swPin = swPin;
  this->_previousState = digitalRead(swPin);
}

bool LimitSwitch::isPressed() {
  return _previousState && !_currentState;
}

bool LimitSwitch::isReleased() {
  return !_previousState && _currentState;
}

void LimitSwitch::tick() {
  _previousState = _currentState;
  _currentState = digitalRead(_swPin);
}

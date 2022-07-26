#pragma once

#include <Arduino.h>

class LimitSwitch
{
public:
  LimitSwitch(uint8_t swPin);

  bool isPressed();

  void tick();
  
  bool wasPressed();

  bool wasReleased();

  void setDebounceTime(unsigned long debounceTime);

private:
  uint8_t _swPin;
  bool _previousState;
  bool _currentState;
  bool _wasPressedAlready;
  bool _wasReleasedAlready;
  unsigned long _debounceTimeInterval;
  unsigned long _lastDebounceTime;
};
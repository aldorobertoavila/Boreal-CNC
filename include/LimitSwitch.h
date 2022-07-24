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
  unsigned long _debounceTime;
};
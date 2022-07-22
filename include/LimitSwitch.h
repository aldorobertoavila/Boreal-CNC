#include <Arduino.h>

class LimitSwitch
{
public:
  LimitSwitch(uint8_t swPin);

  bool isPressed();

  bool isReleased();

  void tick();

private:
  uint8_t _swPin;
  bool _previousState;
  bool _currentState;
};
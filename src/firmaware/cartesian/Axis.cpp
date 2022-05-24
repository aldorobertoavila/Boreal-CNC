#include <Axis.h>

Axis::Axis(AccelStepper *motors) : _motors(motors)
{
    _motors = motors;
}
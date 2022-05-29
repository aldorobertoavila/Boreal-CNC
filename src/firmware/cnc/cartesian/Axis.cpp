#include <Axis.h>

Axis::Axis(AccelStepper *motors, uint8_t size) : _motors(motors), _size(size)
{
    _motors = motors;
    _size = size;
}

uint8_t Axis::getSize()
{
    return _size;
}
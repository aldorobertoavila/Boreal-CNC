#include <Axis.h>

Axis::Axis(AccelStepper *motors, uint8_t size)
{
    this->_motors = motors;
    this->_size = size;
}

uint8_t Axis::getSize()
{
    return _size;
}
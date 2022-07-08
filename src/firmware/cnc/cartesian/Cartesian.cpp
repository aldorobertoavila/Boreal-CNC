#include <Axis.h>
#include <Cartesian.h>

Cartesian::Cartesian(Axis *axes, uint8_t size, Positioning positioning)
{
    this->_acceleration = 0;
    this->_axes = axes;
    this->_positioning = positioning;
    this->_size = size;
    this->_target = {};
}

Axis Cartesian::getAxis(uint8_t index)
{
    return _axes[index];
}

Axis *Cartesian::getAxes()
{
    return _axes;
}

uint8_t Cartesian::getSize()
{
    return _size;
}

void Cartesian::travel()
{
    for (uint8_t i = 0; i < getSize(); i++)
    {
        // TODO: if linear move, set acceleration to 0
        getAxis(i);
    }
}

void Cartesian::setAbsolute()
{
    _positioning = ABSOLUTE;
}

void Cartesian::setAcceleration(float acceleration)
{
    _acceleration = acceleration;
}

void Cartesian::setRelative()
{
    _positioning = RELATIVE;
}

void Cartesian::setTarget(float *target)
{
    _target = target;
}
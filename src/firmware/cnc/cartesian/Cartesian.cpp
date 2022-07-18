#include <Cartesian.h>

Cartesian::Cartesian(Unit &unit) : _unit(unit)
{

}

float Cartesian::getDimensions(Axis axis)
{
    return _dimensions[axis];
}

A4988 *Cartesian::getDriver(Axis axis)
{
    return _drivers[axis];
}

float Cartesian::getHomeOffset(Axis axis)
{
    return _homeOffset[axis];
}

LimitSwitch *Cartesian::getLimitSwitch(Axis axis)
{
    return _switches[axis];
}

Positioning Cartesian::getPositioning()
{
    return _positioning;
}

Unit Cartesian::getUnit()
{
    return _unit;
}

void Cartesian::moveTo(Axis axis, float u)
{
    A4988 *driver = _drivers[axis];

    if(driver)
    {
        uint16_t steps = _unit.toSteps(u, _stepsPerMillimeter[axis]);

        if(_positioning == ABSOLUTE)
        {
            driver->moveTo(steps);
        }
        else
        {
            driver->move(steps);
        }
    }
}

void Cartesian::moveTo(float x, float y, float z)
{
    moveTo(Axis::X, x);
    moveTo(Axis::Y, y);
    moveTo(Axis::Z, z);
}

void Cartesian::run()
{
    for (uint8_t i = 0; i < AXES; i++)
    {
        Axis axis = static_cast<Axis>(i);

        A4988 *driver = getDriver(axis);
        LimitSwitch *sw = getLimitSwitch(axis);

        if(driver && sw)
        {
            driver->run();
            sw->tick();
        }
    }
}

void Cartesian::setDimensions(Axis axis, float u)
{
    _dimensions[axis] = u;
}

void Cartesian::setDriver(Axis axis, A4988 &driver)
{
    _drivers[axis] = &driver;
}

void Cartesian::setHomeOffset(Axis axis, float u)
{
    _homeOffset[axis] = u;
}

void Cartesian::setLimitSwitch(Axis axis, LimitSwitch &sw)
{
    _switches[axis] = &sw;
}

void Cartesian::setPositioning(Positioning positioning)
{
    _positioning = positioning;
}

void Cartesian::setUnit(Unit &unit)
{
    _unit = unit;
}
#include <Cartesian.h>

Cartesian::Cartesian()
{
}

long Cartesian::getDimension(Axis axis)
{
    return _dimensions[axis];
}

long Cartesian::getHomeOffset(Axis axis)
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

Resolution Cartesian::getResolution(Axis axis)
{
    return _resolutions[axis];
}

StepperMotor *Cartesian::getStepperMotor(Axis axis)
{
    return _steppers[axis];
}

uint8_t Cartesian::getStepsPerMillimeter(Axis axis)
{
    return _stepsPerMillimeter[axis];
}

Unit Cartesian::getUnit()
{
    return _unit;
}

void Cartesian::moveTo(Axis axis, float u)
{
    moveTo(axis, _unit, u);
}

void Cartesian::moveTo(Axis axis, Unit unit, float u)
{
    StepperMotor *stepper = _steppers[axis];

    if (stepper)
    {
        long steps = toSteps(axis, unit, u);

        if (_positioning == ABSOLUTE)
        {
            stepper->moveTo(steps);
        }
        else
        {
            stepper->move(steps);
        }

        stepper->setResolution(_resolutions[axis]);
    }
}

void Cartesian::moveTo(float x, float y, float z)
{
    moveTo(Axis::X, x);
    moveTo(Axis::Y, y);
    moveTo(Axis::Z, z);
}

void Cartesian::moveToLimit(Axis axis, Direction dir)
{
    long dimension = getDimension(axis);

    moveTo(axis, Unit::MILLIMETER, dir == Direction::NEGATIVE ? -dimension : dimension);
}

void Cartesian::setDimension(Axis axis, float u)
{
    _dimensions[axis] = u;
}

void Cartesian::setHomeOffset(Axis axis, float u)
{
    _homeOffset[axis] = u;
}

void Cartesian::setLimitSwitch(Axis axis, LimitSwitch &sw)
{
    _switches[axis] = &sw;
}

void Cartesian::setMinStepsPerMillimeter(Axis axis, uint8_t steps)
{
    _minStepsPerMillimeter[axis] = steps;
}

void Cartesian::setPositioning(Positioning positioning)
{
    _positioning = positioning;
}

void Cartesian::setResolution(Axis axis, Resolution res)
{
    _resolutions[axis] = res;
}

void Cartesian::setStepperMotor(Axis axis, StepperMotor &stepper)
{
    _steppers[axis] = &stepper;
}

void Cartesian::setStepsPerMillimeter(Axis axis, uint8_t steps)
{
    _resolutions[axis] = toResolution(steps / _minStepsPerMillimeter[axis]);
    _stepsPerMillimeter[axis] = steps;
}

void Cartesian::setUnit(Unit unit)
{
    _unit = unit;
}

long Cartesian::toSteps(Axis axis, Unit unit, float u)
{
    uint8_t steps = _stepsPerMillimeter[axis];

    switch (unit)
    {
    case MILLIMETER:
        return u * steps;
    case INCH:
        return 25.4 * u * steps;
    default:
        return 0;
    }
}

Resolution Cartesian::toResolution(float factor)
{
    if (factor <= 1)
        return FULL;
    if (factor <= 2)
        return HALF;
    if (factor <= 4)
        return QUARTER;
    if (factor <= 8)
        return EIGHTH;

    return Resolution::SIXTEENTH;
}

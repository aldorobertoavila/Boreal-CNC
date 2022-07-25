#include <Cartesian.h>

Cartesian::Cartesian()
{

}

float Cartesian::getDimensions(Axis axis)
{
    return _dimensions[axis];
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

StepperMotor *Cartesian::getStepperMotor(Axis axis)
{
    return _steppers[axis];
}

Unit Cartesian::getUnit()
{
    return _unit;
}

void Cartesian::moveTo(Axis axis, float u)
{
    StepperMotor *stepper = _steppers[axis];

    if(stepper)
    {
        long steps = toSteps(axis, u);

        if(_positioning == ABSOLUTE)
        {
            stepper->moveTo(steps);
        }
        else
        {
            stepper->move(steps);
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

        StepperMotor *stepper = getStepperMotor(axis);
        LimitSwitch *sw = getLimitSwitch(axis);

        if(stepper && sw)
        {
            stepper->run();
            sw->tick();
        }
    }
}

void Cartesian::setDimensions(Axis axis, float u)
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

void Cartesian::setPositioning(Positioning positioning)
{
    _positioning = positioning;
}

void Cartesian::setStepperMotor(Axis axis, StepperMotor &stepper)
{
    _steppers[axis] = &stepper;
}

void Cartesian::setUnit(Unit unit)
{
    _unit = unit;
}

long Cartesian::toSteps(Axis axis, float u)
{
    uint8_t steps = _stepsPerMillimeter[axis];

    switch (_unit)
    {
    case MILLIMETER:
        return u * steps;
    case INCH:
        return 25.4 * u * steps;
    case CENTIMETER:
        return 10 * u * steps; 
    default:
        return 0;
    }
}

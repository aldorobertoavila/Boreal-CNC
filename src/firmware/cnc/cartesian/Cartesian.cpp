#include <Cartesian.h>

using namespace std;

Cartesian::Cartesian()
{
}

void Cartesian::disableSteppers()
{
  for (uint8_t i = 0; i < AXES; i++)
  {
    StepperMotorPtr stepper = getStepperMotor(static_cast<Axis>(i));

    if (stepper)
    {
      stepper->disable();
    }
  }
}

void Cartesian::enableSteppers()
{
  for (uint8_t i = 0; i < AXES; i++)
  {
    StepperMotorPtr stepper = getStepperMotor(static_cast<Axis>(i));

    if (stepper)
    {
      stepper->enable();
    }
  }
}

void Cartesian::stopSteppers()
{
  for (uint8_t i = 0; i < AXES; i++)
  {
    StepperMotorPtr stepper = getStepperMotor(static_cast<Axis>(i));

    if (stepper)
    {
      stepper->stop();
    }
  }
}

long Cartesian::getDimension(Axis axis)
{
    return _dimensions[axis];
}

long Cartesian::getHomeOffset(Axis axis)
{
    return _homeOffset[axis];
}

LimitSwitchPtr Cartesian::getLimitSwitch(Axis axis)
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

StepperMotorPtr Cartesian::getStepperMotor(Axis axis)
{
    return _steppers[axis];
}

long Cartesian::getStepsPerMillimeter(Axis axis)
{
    return _stepsPerMillimeter[axis];
}

float Cartesian::getTargetPosition(Axis axis)
{
    return _targetPosition[axis];
}

Unit Cartesian::getUnit()
{
    return _unit;
}

void Cartesian::setTargetPosition(Axis axis, float u)
{
    setTargetPosition(axis, _unit, u);
}

void Cartesian::setTargetPosition(Axis axis, Unit unit, float u)
{
    StepperMotorPtr stepper = _steppers[axis];

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

        _targetPosition[axis] = u;

        stepper->setResolution(_resolutions[axis]);
    }
}

void Cartesian::setTargetPosition(float x, float y, float z)
{
    setTargetPosition(Axis::X, x);
    setTargetPosition(Axis::Y, y);
    setTargetPosition(Axis::Z, z);
}

void Cartesian::setDimension(Axis axis, float u)
{
    _dimensions[axis] = u;
}

void Cartesian::setHomeOffset(Axis axis, float u)
{
    _homeOffset[axis] = u;
}

void Cartesian::setLimitSwitch(Axis axis, LimitSwitchPtr sw)
{
    _switches[axis] = sw;
}

void Cartesian::setMinStepsPerMillimeter(Axis axis, long steps)
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

void Cartesian::setStepperMotor(Axis axis, StepperMotorPtr stepper)
{
    _steppers[axis] = stepper;
}

void Cartesian::setStepsPerMillimeter(Axis axis, long steps)
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
    long steps = _stepsPerMillimeter[axis];

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

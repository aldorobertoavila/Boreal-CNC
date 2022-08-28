#include <Cartesian.h>

Cartesian::Cartesian()
{
    this->_positioning = Positioning::ABSOLUTE;
    this->_unit = Unit::MILLIMETER;
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

float Cartesian::getAcceleration(Axis axis)
{
    return _acceleration[axis];
}

Axis Cartesian::getCurrentAxis()
{
    return _currentAxis;
}

float Cartesian::getDimension(Axis axis)
{
    return _dimensions[axis];
}

float Cartesian::getFeedRate(Axis axis)
{
    return _feedRates[axis];
}

float Cartesian::getHomeOffset(Axis axis)
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

float Cartesian::getMaxSpeed(Axis axis)
{
    return _maxSpeed[axis];
}

Unit Cartesian::getUnit()
{
    return _unit;
}

void Cartesian::setAcceleration(Axis axis, Unit unit, float u)
{
    StepperMotorPtr stepper = getStepperMotor(axis);

    if (stepper)
    {
        long accel = toSteps(axis, unit, u);

        _acceleration[axis] = accel;
        stepper->setAcceleration(accel);
    }
}

void Cartesian::setCurrentAxis(Axis axis)
{
    _currentAxis = axis;
}

void Cartesian::setDimension(Axis axis, float u)
{
    _dimensions[axis] = u;
}

void Cartesian::setFeedRate(Axis axis, float feedRate)
{
    // Conversion from mm/min to steps/s
    _feedRates[axis] = toSteps(axis, Unit::MILLIMETER, feedRate / 60);
}

void Cartesian::setHomeOffset(Axis axis, float u)
{
    _homeOffset[axis] = -abs(u);
}

void Cartesian::setLimitSwitch(Axis axis, LimitSwitchPtr sw)
{
    _switches[axis] = sw;
}

void Cartesian::setMaxSpeed(Axis axis, Unit unit, float u)
{
    StepperMotorPtr stepper = getStepperMotor(axis);

    if (stepper)
    {
        long maxSpeed = toSteps(axis, unit, u);

        _maxSpeed[axis] = maxSpeed;
        stepper->setMaxSpeed(maxSpeed);
    }
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

void Cartesian::setTargetPosition(Axis axis, float u, float feedRate)
{
    setTargetPosition(axis, _unit, u, feedRate);
}

void Cartesian::setTargetPosition(Axis axis, Unit unit, float u, float feedRate)
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

        stepper->setMaxSpeed(_feedRates[axis]);
        stepper->setResolution(_resolutions[axis]);
    }
}

void Cartesian::setUnit(Unit unit)
{
    _unit = unit;
}

long Cartesian::toSteps(Axis axis, Unit unit, float u)
{
    long stepsPer = _stepsPerMillimeter[axis];

    switch (unit)
    {
    case Unit::CENTIMETER:
        return 10 * u * stepsPer;
    case Unit::INCH:
        return 25.4 * u * stepsPer;
    case Unit::MICROMETER:
        return (u * stepsPer) / 1000;
    case Unit::MILLIMETER:
        return u * stepsPer;
    default:
        return 0;
    }
}

float Cartesian::toUnit(Axis axis, Unit unit)
{
    StepperMotorPtr stepper = getStepperMotor(axis);

    if (!stepper)
    {
        return 0;
    }

    long steps = stepper->getCurrentPosition();
    long stepsPer = _stepsPerMillimeter[axis];

    switch (unit)
    {
    case Unit::CENTIMETER:
        return steps / (10 * stepsPer);
    case Unit::INCH:
        return steps / (25.4 * stepsPer);
    case Unit::MICROMETER:
        return (steps / stepsPer) * 1000;
    case Unit::MILLIMETER:
        return steps / stepsPer;
    default:
        return 0;
    }
}

Resolution Cartesian::toResolution(float factor)
{
    if (factor <= 1)
    {
        return Resolution::FULL;
    }

    if (factor <= 2)
    {
        return Resolution::HALF;
    }

    if (factor <= 4)
    {
        return Resolution::QUARTER;
    }

    if (factor <= 8)
    {
        return Resolution::EIGHTH;
    }

    return Resolution::SIXTEENTH;
}

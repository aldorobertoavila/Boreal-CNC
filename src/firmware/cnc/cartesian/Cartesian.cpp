#include <Cartesian.h>

Cartesian::Cartesian()
{
    this->_positioning = Positioning::ABSOLUTE;
    this->_unit = LengthUnit::MILLIMETER;
}

void Cartesian::disableSteppers()
{
    for (uint8_t i = 0; i < AXES; i++)
    {
        StepperMotorPtr stepper = getStepperMotor(static_cast<Axis>(i));

        if (stepper && stepper->isEnable())
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

        if (stepper && !stepper->isEnable())
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

float Cartesian::getPrevTargetPosition(Axis axis)
{
    return _prevTargetPos[axis];
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
    return _targetPos[axis];
}

float Cartesian::getMaxSpeed(Axis axis)
{
    return _maxSpeeds[axis];
}

LengthUnit Cartesian::getLengthUnit()
{
    return _unit;
}

void Cartesian::setAcceleration(Axis axis, float u)
{
    setAcceleration(axis, u);
}

void Cartesian::setAcceleration(Axis axis, LengthUnit unit, float u)
{
    StepperMotorPtr stepper = getStepperMotor(axis);

    if (stepper)
    {
        long steps = toSteps(axis, unit, u);

        stepper->setAcceleration(steps);
        _acceleration[axis] = u;
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

void Cartesian::setFeedRate(Axis axis, float u)
{
    setFeedRate(axis, _unit, TimeUnit::MINUTE, u);
}

void Cartesian::setFeedRate(Axis axis, LengthUnit lengthUnit, TimeUnit timeUnit, float u)
{
    StepperMotorPtr stepper = getStepperMotor(axis);

    if (stepper)
    {   
        float time = toTimeUnit(timeUnit, TimeUnit::SECOND, u);
        float constr = constrain(time, 0, _maxSpeeds[axis]);
        float steps = toSteps(axis, lengthUnit, constr);

        stepper->setMaxSpeed(steps);
        _feedRates[axis] = u;
    }
}

void Cartesian::setHomeOffset(Axis axis, float u)
{
    _homeOffset[axis] = -abs(u);
}

void Cartesian::setLimitSwitch(Axis axis, LimitSwitch &sw)
{
    _switches[axis] = &sw;
}

void Cartesian::setMaxSpeed(Axis axis, float u)
{
    setMaxSpeed(axis, _unit, u);
}

void Cartesian::setMaxSpeed(Axis axis, LengthUnit unit, float u)
{
    StepperMotorPtr stepper = getStepperMotor(axis);

    if (stepper)
    {
        long steps = toSteps(axis, unit, u);

        stepper->setMaxSpeed(steps);
        _maxSpeeds[axis] = u;
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

void Cartesian::setStepperMotor(Axis axis, StepperMotor &stepper)
{
    _steppers[axis] = &stepper;
}

void Cartesian::setStepsPerMillimeter(Axis axis, long steps)
{
    _resolutions[axis] = toResolution(steps / _minStepsPerMillimeter[axis]);
    _stepsPerMillimeter[axis] = steps;
}

void Cartesian::setTargetPosition(Axis axis, float u)
{
    setTargetPosition(axis, _unit, u);
}

void Cartesian::setTargetPosition(Axis axis, LengthUnit unit, float u)
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

        _prevTargetPos[axis] = _targetPos[axis];
        _targetPos[axis] = u;

        stepper->setResolution(_resolutions[axis]);
    }
}

void Cartesian::setLengthUnit(LengthUnit unit)
{
    // if same, don't recalculate
    if (_unit == unit)
    {
        return;
    }

    for (uint8_t i = 0; i < AXES; i++)
    {
        Axis axis = static_cast<Axis>(i);

        setDimension(axis, toLengthUnit(_unit, unit, _dimensions[axis]));
        setHomeOffset(axis, toLengthUnit(_unit, unit, _homeOffset[axis]));

        setAcceleration(axis, unit, toLengthUnit(_unit, unit, _acceleration[axis]));
        setMaxSpeed(axis, unit, toLengthUnit(_unit, unit, _maxSpeeds[axis]));
        setFeedRate(axis, unit, TimeUnit::SECOND, toLengthUnit(_unit, unit, _feedRates[axis]));
        setTargetPosition(axis, unit, toLengthUnit(_unit, unit, _targetPos[axis]));
    }

    _unit = unit;
}

long Cartesian::toSteps(Axis axis, LengthUnit unit, float u)
{
    return _stepsPerMillimeter[axis] * toLengthUnit(unit, LengthUnit::MILLIMETER, u);
}

float Cartesian::toLengthUnit(Axis axis, LengthUnit unit, long steps)
{
    return steps / (toLengthUnit(unit, LengthUnit::MILLIMETER, 1) * _stepsPerMillimeter[axis]);
}

float Cartesian::toLengthUnit(LengthUnit from, LengthUnit to, float u)
{
    switch (from)
    {
    case CENTIMETER:
        switch (to)
        {
        case MILLIMETER:
            return u * 10;
        case MICROMETER:
            return u * 10000;
        case INCH:
            return u / 2.54;
        }
        break;
    case MILLIMETER:
        switch (to)
        {
        case CENTIMETER:
            return u / 10;
        case MICROMETER:
            return u * 1000;
        case INCH:
            return u / 25.4;
        }
        break;
    case MICROMETER:
        switch (to)
        {
        case CENTIMETER:
            return u / 10000;
        case MILLIMETER:
            return u / 1000;
        case INCH:
            return u / 25400;
        }
        break;
    case INCH:
        switch (to)
        {
        case CENTIMETER:
            return u * 2.54;
        case MILLIMETER:
            return u * 25.4;
        case MICROMETER:
            return u * 25400;
        }
        break;
    }

    return u; // if no conversion needed
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

float Cartesian::toTimeUnit(TimeUnit from, TimeUnit to, float u)
{
    switch (from)
    {
    case SECOND:
        switch (from)
        {
        case MINUTE:
            return u * 60;
        }
        break;
    case MINUTE:
        switch (from)
        {
        case SECOND:
            return u / 60;
        }
        break;
    }

    return u; // if no conversion needed
}
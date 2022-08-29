#pragma once

#include <Axis.h>
#include <LimitSwitch.h>
#include <Positioning.h>
#include <Resolution.h>
#include <StepperMotor.h>
#include <Unit.h>

#include <memory>

using namespace std;

using LimitSwitchPtr = LimitSwitch*;
using StepperMotorPtr = StepperMotor*;

class Cartesian
{
public:
    Cartesian();

    void disableSteppers();

    void enableSteppers();

    void stopSteppers();

    float getAcceleration(Axis axis);

    Axis getCurrentAxis();

    float getDimension(Axis axis);

    float getFeedRate(Axis axis);

    float getHomeOffset(Axis axis);

    float getMaxSpeed(Axis axis);

    LimitSwitchPtr getLimitSwitch(Axis axis);

    Positioning getPositioning();

    Resolution getResolution(Axis axis);

    StepperMotorPtr getStepperMotor(Axis axis);

    long getStepsPerMillimeter(Axis axis);

    float getTargetPosition(Axis axis);

    Unit getUnit();

    void setAcceleration(Axis axis, Unit unit, float u);

    void setCurrentAxis(Axis axis);

    void setDimension(Axis axis, float u);

    void setFeedRate(Axis axis, float feedRate);

    void setHomeOffset(Axis axis, float u);

    void setMaxSpeed(Axis axis, Unit unit, float u);

    void setMinStepsPerMillimeter(Axis axis, long steps);

    void setLimitSwitch(Axis axis, LimitSwitch &sw);

    void setPositioning(Positioning positioning);

    void setResolution(Axis axis, Resolution res);

    void setStepperMotor(Axis axis, StepperMotor &stepper);

    void setStepsPerMillimeter(Axis axis, long steps);

    void setTargetPosition(Axis axis, float u, float feedRate);

    void setTargetPosition(Axis axis, Unit unit, float u, float feedRate);

    void setUnit(Unit unit);

    long toSteps(Axis axis, Unit unit, float u);

    float toUnit(Axis axis, Unit unit);

protected:
    Resolution toResolution(float factor);

private:
    StepperMotorPtr _steppers[AXES];
    LimitSwitchPtr _switches[AXES];
    Resolution _resolutions[AXES];
    Positioning _positioning;
    Unit _unit;
    Axis _currentAxis;
    float _acceleration[AXES];
    float _dimensions[AXES];
    float _feedRates[AXES];
    float _homeOffset[AXES];
    float _maxSpeeds[AXES];
    float _targetPosition[AXES];
    long _minStepsPerMillimeter[AXES];
    long _stepsPerMillimeter[AXES];
};

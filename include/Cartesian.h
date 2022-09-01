#pragma once

#include <Axis.h>
#include <LimitSwitch.h>
#include <Positioning.h>
#include <Resolution.h>
#include <StepperMotor.h>
#include <Unit.h>

#include <memory>

using namespace std;

using LimitSwitchPtr = LimitSwitch *;
using StepperMotorPtr = StepperMotor *;

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

    float getPrevTargetPosition(Axis axis);

    Resolution getResolution(Axis axis);

    StepperMotorPtr getStepperMotor(Axis axis);

    long getStepsPerMillimeter(Axis axis);

    float getTargetPosition(Axis axis);

    LengthUnit getLengthUnit();

    void setAcceleration(Axis axis, float u);

    void setAcceleration(Axis axis, LengthUnit unit, float u);

    void setCurrentAxis(Axis axis);

    void setDimension(Axis axis, float u);

    void setFeedRate(Axis axis, float u);

    void setFeedRate(Axis axis, LengthUnit lengthUnit, TimeUnit timeUnit, float u);

    void setHomeOffset(Axis axis, float u);

    void setMaxSpeed(Axis axis, float u);

    void setMaxSpeed(Axis axis, LengthUnit unit, float u);

    void setMinStepsPerMillimeter(Axis axis, long steps);

    void setLimitSwitch(Axis axis, LimitSwitch &sw);

    void setPositioning(Positioning positioning);

    void setResolution(Axis axis, Resolution res);

    void setStepperMotor(Axis axis, StepperMotor &stepper);

    void setStepsPerMillimeter(Axis axis, long steps);

    void setTargetPosition(Axis axis, float u);

    void setTargetPosition(Axis axis, LengthUnit unit, float u);

    void setLengthUnit(LengthUnit unit);

    long toSteps(Axis axis, LengthUnit unit, float u);

    float toLengthUnit(Axis axis, LengthUnit unit, long steps);

    float toLengthUnit(LengthUnit from, LengthUnit to, float u);

protected:
    Resolution toResolution(float factor);

    float toTimeUnit(TimeUnit from, TimeUnit to, float u);

private:
    StepperMotorPtr _steppers[AXES];
    LimitSwitchPtr _switches[AXES];
    Resolution _resolutions[AXES];
    Positioning _positioning;
    LengthUnit _unit;
    Axis _currentAxis;
    float _acceleration[AXES];
    float _dimensions[AXES];
    float _feedRates[AXES];
    float _homeOffset[AXES];
    float _maxSpeeds[AXES];
    float _prevTargetPos[AXES];
    float _targetPos[AXES];
    long _minStepsPerMillimeter[AXES];
    long _stepsPerMillimeter[AXES];
};

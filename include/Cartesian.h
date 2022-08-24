#pragma once

#include <Axis.h>
#include <LimitSwitch.h>
#include <Positioning.h>
#include <Resolution.h>
#include <StepperMotor.h>
#include <Unit.h>

#include <memory>

using namespace std;

using LimitSwitchPtr = std::shared_ptr<LimitSwitch>;
using StepperMotorPtr = std::shared_ptr<StepperMotor>;

class Cartesian
{
public:
    Cartesian();

    void disableSteppers();

    void enableSteppers();

    void stopSteppers();

    long getDimension(Axis axis);

    long getHomeOffset(Axis axis);

    LimitSwitchPtr getLimitSwitch(Axis axis);

    Positioning getPositioning();

    Resolution getResolution(Axis axis);

    StepperMotorPtr getStepperMotor(Axis axis);

    long getStepsPerMillimeter(Axis axis);

    float getTargetPosition(Axis axis);

    Unit getUnit();

    void setTargetPosition(Axis axis, float u);

    void setTargetPosition(Axis axis, Unit unit, float u);

    void setTargetPosition(float x, float y, float z);

    void setDimension(Axis axis, float u);

    void setHomeOffset(Axis axis, float u);

    void setMinStepsPerMillimeter(Axis axis, long steps);

    void setLimitSwitch(Axis axis, LimitSwitchPtr sw);

    void setPositioning(Positioning positioning);

    void setResolution(Axis axis, Resolution res);

    void setStepperMotor(Axis axis, StepperMotorPtr stepper);

    void setStepsPerMillimeter(Axis axis, long steps);

    void setUnit(Unit unit);

protected:
    Resolution toResolution(float factor);

    long toSteps(Axis axis, Unit unit, float u);

private:
    StepperMotorPtr _steppers[AXES];
    LimitSwitchPtr _switches[AXES];
    Resolution _resolutions[AXES];
    Positioning _positioning;
    Unit _unit;
    long _dimensions[AXES];
    long _homeOffset[AXES];
    long _minStepsPerMillimeter[AXES];
    long _stepsPerMillimeter[AXES];
    float _targetPosition[AXES];
};

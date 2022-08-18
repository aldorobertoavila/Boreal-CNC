#pragma once

#include <Axis.h>
#include <Direction.h>
#include <LimitSwitch.h>
#include <Positioning.h>
#include <Resolution.h>
#include <StepperMotor.h>
#include <Unit.h>

class Cartesian
{
public:
    Cartesian();

    long getDimension(Axis axis);

    long getHomeOffset(Axis axis);

    LimitSwitch *getLimitSwitch(Axis axis);

    Positioning getPositioning();

    Resolution getResolution(Axis axis);

    StepperMotor *getStepperMotor(Axis axis);

    long getStepsPerMillimeter(Axis axis);

    float getTargetPosition(Axis axis);

    Unit getUnit();

    void setTargetPosition(Axis axis, float u);

    void setTargetPosition(Axis axis, Unit unit, float u);

    void setTargetPosition(float x, float y, float z);

    void setTargetPosition(Axis axis, Direction dir);

    void setDimension(Axis axis, float u);

    void setHomeOffset(Axis axis, float u);

    void setMinStepsPerMillimeter(Axis axis, long steps);

    void setLimitSwitch(Axis axis, LimitSwitch &sw);

    void setPositioning(Positioning positioning);

    void setResolution(Axis axis, Resolution res);

    void setStepperMotor(Axis axis, StepperMotor &stepper);

    void setStepsPerMillimeter(Axis axis, long steps);

    void setUnit(Unit unit);

protected:
    Resolution toResolution(float factor);

    long toSteps(Axis axis, Unit unit, float u);

private:
    StepperMotor *_steppers[AXES];
    LimitSwitch *_switches[AXES];
    Resolution _resolutions[AXES];
    Positioning _positioning;
    Unit _unit;
    long _dimensions[AXES];
    long _homeOffset[AXES];
    long _minStepsPerMillimeter[AXES];
    long _stepsPerMillimeter[AXES];
    float _targetPosition[AXES];
};

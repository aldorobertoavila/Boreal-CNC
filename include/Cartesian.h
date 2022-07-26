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

    uint8_t getStepsPerMillimeter(Axis axis);

    Unit getUnit();

    void moveTo(Axis axis, float u);

    void moveTo(Axis axis, Unit unit, float u);

    void moveTo(float x, float y, float z);

    void moveToLimit(Axis axis, Direction dir);

    void setDimension(Axis axis, float u);

    void setHomeOffset(Axis axis, float u);

    void setMinStepsPerMillimeter(Axis axis, uint8_t steps);

    void setLimitSwitch(Axis axis, LimitSwitch &sw);

    void setPositioning(Positioning positioning);

    void setResolution(Axis axis, Resolution res);

    void setStepperMotor(Axis axis, StepperMotor &stepper);

    void setStepsPerMillimeter(Axis axis, uint8_t steps);

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
    uint8_t _minStepsPerMillimeter[AXES];
    uint8_t _stepsPerMillimeter[AXES];
};

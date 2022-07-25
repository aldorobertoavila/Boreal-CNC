#pragma once

#include <Axis.h>
#include <LimitSwitch.h>
#include <Positioning.h>
#include <Resolution.h>
#include <StepperMotor.h>
#include <Unit.h>

#define AXES 3

class Cartesian
{
public:
    Cartesian();

    float getDimension(Axis axis);

    float getHomeOffset(Axis axis);

    LimitSwitch *getLimitSwitch(Axis axis);

    Positioning getPositioning();

    Resolution getResolution(Axis axis);

    StepperMotor *getStepperMotor(Axis axis);

    uint8_t getStepsPerMillimeter(Axis axis);

    Unit getUnit();

    void run();

    void moveTo(Axis axis, float u);

    void moveTo(Axis axis, Unit unit, float u);

    void moveTo(float x, float y, float z);

    void setDimension(Axis axis, float u);

    void setHomeOffset(Axis axis, float u);

    void setLimitSwitch(Axis axis, LimitSwitch &sw);

    void setPositioning(Positioning positioning);

    void setResolution(Axis axis, Resolution res);

    void setStepperMotor(Axis axis, StepperMotor &stepper);

    void setStepsPerMillimeter(Axis axis, uint8_t steps);

    void setUnit(Unit unit);

protected:
    long toSteps(Axis axis, Unit unit, float u);

private:
    StepperMotor *_steppers[AXES];
    LimitSwitch *_switches[AXES];
    Resolution _resolutions[AXES];
    Positioning _positioning;
    Unit _unit;
    float _dimensions[AXES];
    float _homeOffset[AXES];
    uint8_t _stepsPerMillimeter[AXES];
};

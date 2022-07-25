#include <Axis.h>
#include <LimitSwitch.h>
#include <Positioning.h>
#include <StepperMotor.h>
#include <Unit.h>

#define AXES 3

class Cartesian
{
public:
    Cartesian();

    float getDimensions(Axis axis);

    float getHomeOffset(Axis axis);

    LimitSwitch *getLimitSwitch(Axis axis);

    Positioning getPositioning();

    StepperMotor *getStepperMotor(Axis axis);

    Unit getUnit();

    void run();

    void moveTo(Axis axis, float u);

    void moveTo(float x, float y, float z);

    void setDimensions(Axis axis, float u);

    void setHomeOffset(Axis axis, float u);

    void setLimitSwitch(Axis axis, LimitSwitch &sw);

    void setPositioning(Positioning positioning);

    void setStepperMotor(Axis axis, StepperMotor &stepper);

    void setUnit(Unit unit);

protected:
    long toSteps(Axis axis, float u);

private:
    StepperMotor *_steppers[AXES];
    LimitSwitch *_switches[AXES];
    Positioning _positioning;
    Unit _unit;
    float _dimensions[AXES];
    float _homeOffset[AXES];
    uint8_t _stepsPerMillimeter[AXES];
};

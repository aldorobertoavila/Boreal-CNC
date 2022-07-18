#include <A4988.h>
#include <Axis.h>
#include <LimitSwitch.h>
#include <Positioning.h>
#include <Unit.h>

#define AXES 3

class Cartesian
{
public:
    Cartesian(Unit &unit);

    float getDimensions(Axis axis);

    A4988 *getDriver(Axis axis);

    float getHomeOffset(Axis axis);

    LimitSwitch *getLimitSwitch(Axis axis);

    Positioning getPositioning();

    Unit getUnit();

    void run();

    void moveTo(Axis axis, float u);

    void moveTo(float x, float y, float z);

    void setDimensions(Axis axis, float u);

    void setDriver(Axis axis, A4988 &driver);

    void setHomeOffset(Axis axis, float u);

    void setLimitSwitch(Axis axis, LimitSwitch &sw);

    void setPositioning(Positioning positioning);

    void setUnit(Unit &unit);

private:
    A4988 *_drivers[AXES];
    LimitSwitch *_switches[AXES];
    Positioning _positioning;
    float _dimensions[AXES];
    float _homeOffset[AXES];
    uint8_t _stepsPerMillimeter[AXES];
    Unit &_unit;
};

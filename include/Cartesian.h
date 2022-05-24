#ifndef Cartesian_h
#define Cartesian_h

#include <Axis.h>

class Cartesian
{
public:
    enum Positioning
    {
        ABSOLUTE,
        RELATIVE
    };

    Cartesian(Axis *axes, byte size, Positioning positioning);

    Axis getAxis(byte index);

    Axis *getAxes();

    byte getSize();

    void travel();

    void setAbsolute();

    void setAcceleration(float acceleration);

    void setRelative();

    void setTarget(float *target);

private:
    float _acceleration;
    Axis *_axes;
    byte _size;
    float *_target;
    Positioning _positioning;
};

#endif
#include <Axis.h>

class Cartesian
{
public:
    enum Positioning
    {
        ABSOLUTE,
        RELATIVE
    };

    Cartesian(Axis *axes, uint8_t size);

    Axis getAxis(uint8_t index);

    Axis *getAxes();

    uint8_t getSize();

    void travel();

    void setAbsolute();

    void setAcceleration(float acceleration);

    void setRelative();

    void setTarget(float *target);

private:
    float _acceleration;
    Axis *_axes;
    uint8_t _size;
    float *_target;
    Positioning _positioning;
};
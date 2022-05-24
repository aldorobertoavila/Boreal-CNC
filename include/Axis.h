#ifndef Axis_h
#define Axis_h

#include <AccelStepper.h>

class Axis
{
public:
    Axis(AccelStepper *motors);

private:
    AccelStepper *_motors;
};

#endif
#ifndef Axis_h
#define Axis_h

#include <AccelStepper.h>

class Axis
{
public:
    Axis(AccelStepper *motors, uint8_t size = 1);

    uint8_t getSize();

private:
    AccelStepper *_motors;
    uint8_t _size;
};

#endif
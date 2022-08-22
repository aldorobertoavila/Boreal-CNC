#pragma once

#define AXES 3

enum Axis
{
    X,
    Y,
    Z
};

inline Axis operator++(Axis &axis, int)
{
    Axis currentAxis = axis;

    if (Axis::Z < axis + 1)
    {
        axis = Axis::X;
    }
    else
    {
        axis = static_cast<Axis>(axis + 1);
    }

    return currentAxis;
}
#include <Cartesian.h>
#include <Command.h>

AutohomeCommand::AutohomeCommand(Cartesian &cartesian) : _cartesian(cartesian)
{
}

void AutohomeCommand::execute()
{
    _cartesian.run();
}

void AutohomeCommand::start()
{
    float x = _cartesian.getDimensions(Axis::X);
    float y = _cartesian.getDimensions(Axis::Y);
    float z = _cartesian.getDimensions(Axis::Z);

    _cartesian.moveTo(-x, -y, -z);
}

Status AutohomeCommand::status()
{
    LimitSwitch *x = _cartesian.getLimitSwitch(Axis::X);
    LimitSwitch *y = _cartesian.getLimitSwitch(Axis::Y);
    LimitSwitch *z = _cartesian.getLimitSwitch(Axis::Z);

    if(!x || !y || !z)
    {
        return ERROR;
    }

    if(x->isPressed() && y->isPressed() && z->isPressed())
    {
        return COMPLETED;
    }

    return CONTINUE;
}

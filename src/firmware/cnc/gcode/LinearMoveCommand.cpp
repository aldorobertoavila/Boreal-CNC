#include <Axis.h>
#include <Command.h>

LinearMoveCommand::LinearMoveCommand(Cartesian &cartesian, float x, float y, float z) : _cartesian(cartesian)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
}

void LinearMoveCommand::execute()
{
    _cartesian.run();
}

void LinearMoveCommand::start()
{
    _cartesian.moveTo(_x, _y, _z);
}

CommandStatus LinearMoveCommand::status()
{
    StepperMotor *x = _cartesian.getStepperMotor(Axis::X);
    StepperMotor *y = _cartesian.getStepperMotor(Axis::Y);
    StepperMotor *z = _cartesian.getStepperMotor(Axis::Z);

    if(!x || !y || !z)
    {
        return ERROR;
    }

    if(x && y && z)
    {
        if(x->distanceTo() == 0 && y->distanceTo() == 0 && z->distanceTo() == 0) return COMPLETED;
    }

    return CONTINUE;
}
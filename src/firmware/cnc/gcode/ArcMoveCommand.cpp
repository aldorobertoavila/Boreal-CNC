#include <Command.h>

ArcMoveCommand::ArcMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float i, float j, float k, float feedRate, uint8_t power) : MoveCommand(cartesian, laser, feedRate, power)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_i = i;
    this->_j = j;
    this->_k = k;
}

void ArcMoveCommand::execute()
{
    complete();
}

void ArcMoveCommand::setup()
{
    _currentStatus = Status::CONTINUE;

    if (_laser.getInlineMode() == InlineMode::OFF)
    {
        _laser.setPower(_power);
    }

    _laser.turnOn();
}

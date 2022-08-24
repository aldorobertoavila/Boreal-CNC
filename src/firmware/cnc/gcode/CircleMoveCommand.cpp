#include <Command.h>

CircleMoveCommand::CircleMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float r, float feedRate, uint8_t power) : _cartesian(cartesian), _laser(laser), MoveCommand(cartesian, laser, feedRate, power)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_r = r;
}

void CircleMoveCommand::execute()
{
    complete();
}

void CircleMoveCommand::setup()
{
    _currentStatus = Status::CONTINUE;

    if (_laser.getInlineMode() == InlineMode::OFF)
    {
        _laser.setPower(_power);
    }

    _laser.turnOn();
}

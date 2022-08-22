#include <Command.h>

CircleMoveCommand::CircleMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float r, float feedrate, uint8_t power) : _cartesian(cartesian), _laser(laser)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_r = r;
    this->_feedrate = feedrate;
    this->_power = power;
}

void CircleMoveCommand::execute()
{
}

void CircleMoveCommand::finish()
{
    if (_power > 0)
    {
        _laser.turnOff();
    }
}

void CircleMoveCommand::start()
{
    _currentStatus = Status::CONTINUE;

    if (_laser.getInlineMode() == InlineMode::OFF)
    {
        _laser.setPower(_power);
    }

    _laser.turnOn();
}

#include <Command.h>

ArcMoveCommand::ArcMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float i, float j, float k, float feedrate, uint8_t power) : _cartesian(cartesian), _laser(laser)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_i = i;
    this->_j = j;
    this->_k = k;
    this->_feedrate = feedrate;
    this->_power = power;
}

void ArcMoveCommand::execute()
{
}

void ArcMoveCommand::finish()
{
    if (_power > 0)
    {
        _laser.turnOff();
    }
}

void ArcMoveCommand::start()
{
    _currentStatus = Status::CONTINUE;

    if (_laser.getInlineMode() == InlineMode::OFF)
    {
        _laser.setPower(_power);
    }

    _laser.turnOn();
}

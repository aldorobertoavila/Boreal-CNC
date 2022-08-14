#include <Command.h>

CircleMoveCommand::CircleMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float r, uint8_t s) : _cartesian(cartesian), _laser(laser)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_r = r;
    this->_s = s;
}

void CircleMoveCommand::execute()
{
}

void CircleMoveCommand::finish()
{
    if (_s > 0 && _laser.getInlineMode() == OFF)
    {
        _laser.turnOff();
    }
}

void CircleMoveCommand::start()
{
    if (_laser.getInlineMode() == OFF)
    {
        _laser.setPower(_s);
    }

    _laser.turnOn();
}

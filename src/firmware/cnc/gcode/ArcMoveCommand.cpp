#include <Command.h>

ArcMoveCommand::ArcMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float i, float j, float k, uint8_t s) : _cartesian(cartesian), _laser(laser)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_i = i;
    this->_j = j;
    this->_k = k;
    this->_s = s;
}

void ArcMoveCommand::execute()
{
}

void ArcMoveCommand::finish()
{
    if (_s > 0 && _laser.getInlineMode() == OFF)
    {
        _laser.turnOff();
    }
}

void ArcMoveCommand::start()
{
    if (_laser.getInlineMode() == OFF)
    {
        _laser.setPower(_s);
    }

    _laser.turnOn();
}

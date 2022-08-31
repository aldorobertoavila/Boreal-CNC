#include <Command.h>

CircleMoveCommand::CircleMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float r, float feedRate, uint8_t power) : _cartesian(cartesian), _laser(laser)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_r = r;
    this->_feedRate = feedRate;
    this->_power = power;
}

bool CircleMoveCommand::continues()
{
    return false;
}

void CircleMoveCommand::execute()
{
}

void CircleMoveCommand::setup()
{
}

void CircleMoveCommand::stop()
{
}

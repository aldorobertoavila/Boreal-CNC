#include <Command.h>

ArcMoveCommand::ArcMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float i, float j, float k, float feedRate, uint8_t power) : _cartesian(cartesian), _laser(laser)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_i = i;
    this->_j = j;
    this->_k = k;
    this->_feedRate = feedRate;
    this->_power = power;
}

bool ArcMoveCommand::continues()
{
    return false;
}

void ArcMoveCommand::execute()
{
}

void ArcMoveCommand::setup()
{
}

void ArcMoveCommand::stop()
{
}
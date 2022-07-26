#include <Command.h>

ArcMoveCommand::ArcMoveCommand(Cartesian &cartesian, float x, float y, float z, float i, float j, float k, float r) : _cartesian(cartesian)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_i = i;
    this->_j = j;
    this->_k = k;
    this->_r = r;
}

void ArcMoveCommand::execute()
{

}

void ArcMoveCommand::start()
{

}

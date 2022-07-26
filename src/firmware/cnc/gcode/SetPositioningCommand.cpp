#include <Command.h>

SetPositioningCommand::SetPositioningCommand(Cartesian &cartesian, Positioning pos) : _cartesian(cartesian)
{
    this->_cartesian = cartesian;
    this->_pos = pos;
}

void SetPositioningCommand::execute()
{
    _cartesian.setPositioning(_pos);
}

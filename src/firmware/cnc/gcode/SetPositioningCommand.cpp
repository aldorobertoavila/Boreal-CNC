#include <Command.h>

SetPositioningCommand::SetPositioningCommand(Cartesian &cartesian, Positioning pos) : _cartesian(cartesian)
{
    this->_cartesian = cartesian;
    this->_positioning = pos;
}

bool SetPositioningCommand::continues()
{
    return _cartesian.getPositioning() != _positioning;
}

void SetPositioningCommand::execute()
{
    _cartesian.setPositioning(_positioning);
}

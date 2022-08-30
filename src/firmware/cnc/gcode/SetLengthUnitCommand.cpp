#include <Command.h>

SetLengthUnitCommand::SetLengthUnitCommand(Cartesian &cartesian, LengthUnit unit) : _cartesian(cartesian)
{
    this->_cartesian = cartesian;
    this->_unit = unit;
}

bool SetLengthUnitCommand::continues()
{
    return _cartesian.getLengthUnit() != _unit;
}


void SetLengthUnitCommand::execute()
{
    _cartesian.setLengthUnit(_unit);
}

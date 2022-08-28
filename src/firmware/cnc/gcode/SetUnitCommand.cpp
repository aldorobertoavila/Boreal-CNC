#include <Command.h>

SetUnitCommand::SetUnitCommand(Cartesian &cartesian, Unit unit) : _cartesian(cartesian)
{
    this->_cartesian = cartesian;
    this->_unit = unit;
}

bool SetUnitCommand::continues()
{
    return _cartesian.getUnit() != _unit;
}


void SetUnitCommand::execute()
{
    _cartesian.setUnit(_unit);
}

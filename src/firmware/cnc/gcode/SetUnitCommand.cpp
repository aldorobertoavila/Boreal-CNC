#include <Command.h>

SetUnitCommand::SetUnitCommand(Cartesian &cartesian, Unit unit) : _cartesian(cartesian)
{
    this->_cartesian = cartesian;
    this->_unit = unit;
}

void SetUnitCommand::execute()
{
    _cartesian.setUnit(_unit);
}

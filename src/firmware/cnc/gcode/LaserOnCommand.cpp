#include <Command.h>

LaserOnCommand::LaserOnCommand(Laser &laser, uint8_t power, InlineMode mode) : _laser(laser)
{
    this->_power = power;
    this->_mode = mode;
}

bool LaserOnCommand::continues()
{
    return _power > 0 ? !_laser.isTurnOn() : false;
}

void LaserOnCommand::execute()
{
    _laser.setInlineMode(_mode);
    _laser.setPower(_power);
    _laser.turnOn();
}
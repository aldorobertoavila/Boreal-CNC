#include <Command.h>

LaserOffCommand::LaserOffCommand(Laser &laser) : _laser(laser)
{
}

bool LaserOffCommand::continues()
{
    return _laser.isTurnOn();
}

void LaserOffCommand::execute()
{
    _laser.setInlineMode(OFF);
    _laser.turnOff();
}
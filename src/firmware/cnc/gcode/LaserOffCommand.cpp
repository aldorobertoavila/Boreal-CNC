#include <Command.h>

LaserOffCommand::LaserOffCommand(Laser &laser) : _laser(laser)
{
}

void LaserOffCommand::execute()
{
    _laser.setInlineMode(OFF);
    _laser.turnOff();
}
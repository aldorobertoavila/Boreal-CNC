#include <Command.h>

MoveCommand::MoveCommand(Cartesian &cartesian, Laser &laser, float feedRate, uint8_t power) : _cartesian(cartesian), _laser(laser)
{
    this->_feedRate = feedRate;
    this->_power = power;
}

void MoveCommand::complete()
{
    _cartesian.stopSteppers();

    if (_power > 0)
    {
        _laser.turnOff();
    }

    _currentStatus = Status::COMPLETED;
};

void MoveCommand::stop()
{
    _cartesian.stopSteppers();
    _laser.turnOff();

    _currentStatus = Status::STOPPED;
};

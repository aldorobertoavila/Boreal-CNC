#include <Command.h>

LinearMoveCommand::LinearMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, uint8_t s) : _cartesian(cartesian), _laser(laser)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_s = s;
}

void LinearMoveCommand::execute()
{
    StepperMotor *stepper = _cartesian.getStepperMotor(_currentAxis);
    LimitSwitch *sw = _cartesian.getLimitSwitch(_currentAxis);

    stepper->run();
    sw->tick();

    if (stepper->distanceTo() == 0 || sw->wasPressed() || sw->isPressed())
    {
        if (_currentAxis == Axis::Z)
        {
            _currentStatus = Status::COMPLETED;
            return;
        }

        _currentAxis++;
    }
}

void LinearMoveCommand::finish()
{
    if (_s > 0 && _laser.getInlineMode() == OFF)
    {
        _laser.turnOff();
    }
}

void LinearMoveCommand::start()
{
    _currentAxis = Axis::X;

    if (_laser.getInlineMode() == OFF)
    {
        _laser.setPower(_s);
    }

    _cartesian.moveTo(_x, _y, _z);
    _laser.turnOn();
}
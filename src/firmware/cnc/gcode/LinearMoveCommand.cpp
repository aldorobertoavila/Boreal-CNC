#include <Command.h>

LinearMoveCommand::LinearMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float feedrate, uint8_t power) : _cartesian(cartesian), _laser(laser)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_feedrate = feedrate;
    this->_power = power;
}

void LinearMoveCommand::execute()
{
    StepperMotorPtr stepper = _cartesian.getStepperMotor(_currentAxis);
    LimitSwitchPtr sw = _cartesian.getLimitSwitch(_currentAxis);

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
    if (_power > 0)
    {
        _laser.turnOff();
    }
}

void LinearMoveCommand::start()
{
    _currentAxis = Axis::X;
    _currentStatus = Status::CONTINUE;

    if (_laser.getInlineMode() == OFF)
    {
        _laser.setPower(_power);
    }

    if (_x > 0)
    {
        _cartesian.setTargetPosition(Axis::X, _x);
    }

    if (_y > 0)
    {
        _cartesian.setTargetPosition(Axis::Y, _y);
    }

    if (_z > 0)
    {
        _cartesian.setTargetPosition(Axis::Z, _z);
    }

    _laser.turnOn();
}
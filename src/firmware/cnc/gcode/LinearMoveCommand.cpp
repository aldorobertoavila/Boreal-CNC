#include <Command.h>

LinearMoveCommand::LinearMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float feedRate, uint8_t power) : _cartesian(cartesian), _laser(laser)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_feedRate = feedRate;
    this->_power = power;
}

bool LinearMoveCommand::continues()
{
    StepperMotorPtr stepperX = _cartesian.getStepperMotor(Axis::X);
    StepperMotorPtr stepperY = _cartesian.getStepperMotor(Axis::Y);
    StepperMotorPtr stepperZ = _cartesian.getStepperMotor(Axis::Z);

    if (!stepperX || !stepperY || !stepperZ)
    {
        return false;
    }

    return stepperX->distanceTo() != 0 || stepperY->distanceTo() != 0 || stepperZ->distanceTo() != 0;
}

void LinearMoveCommand::execute()
{
    Axis axis = _cartesian.getCurrentAxis();

    StepperMotorPtr stepper = _cartesian.getStepperMotor(axis);
    LimitSwitchPtr sw = _cartesian.getLimitSwitch(axis);

    stepper->run();
    sw->tick();

    if (stepper->distanceTo() == 0)
    {
        axis++;
        _cartesian.setCurrentAxis(axis);
    }
}

void LinearMoveCommand::setup()
{
    _cartesian.enableSteppers();
    _cartesian.setCurrentAxis(Axis::X);

    if (_x > 0)
    {
        if(_feedRate > 0)
        {
            _cartesian.setFeedRate(Axis::X, _feedRate);
        }
        
        _cartesian.setTargetPosition(Axis::X, _x);
    }

    if (_y > 0)
    {
        if(_feedRate > 0)
        {
            _cartesian.setFeedRate(Axis::Y, _feedRate);
        }
        
        _cartesian.setTargetPosition(Axis::Y, _y);
    }

    if (_z > 0)
    {
        if(_feedRate > 0)
        {
            _cartesian.setFeedRate(Axis::Z, _feedRate);
        }
        
        _cartesian.setTargetPosition(Axis::Z, _z);
    }

    if (!_laser.isTurnOn() && _laser.getInlineMode() == ON)
    {
        _laser.turnOn();
    }

    if (_power > 0)
    {
        _laser.setPower(_power);
    }
}

void LinearMoveCommand::stop()
{
    if (_power > 0 && _laser.isTurnOn())
    {
        _laser.turnOff();
    }
}
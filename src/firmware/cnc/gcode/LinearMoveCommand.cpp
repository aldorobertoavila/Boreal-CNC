#include <Command.h>

LinearMoveCommand::LinearMoveCommand(Cartesian &cartesian, float x, float y, float z) : _cartesian(cartesian)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
}

void LinearMoveCommand::execute()
{
    bool completed = HIGH;

    for (uint8_t i = 0; i < AXES; i++)
    {
        Axis axis = static_cast<Axis>(i);

        StepperMotor *stepper = _cartesian.getStepperMotor(axis);
        LimitSwitch *sw = _cartesian.getLimitSwitch(axis);

        if (stepper && sw)
        {
            stepper->run();
            sw->tick();

            if(stepper->distanceTo() == 0 || sw->wasPressed() || sw->isPressed())
            {
                completed &= HIGH;
            }
        }
    }

    if(completed)
    {
        _currentStatus = CommandStatus::COMPLETED;
    }
}

void LinearMoveCommand::start()
{
    _cartesian.moveTo(_x, _y, _z);
}

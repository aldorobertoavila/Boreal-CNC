#include <Command.h>

AutohomeCommand::AutohomeCommand(Cartesian &cartesian, Laser &laser) : _cartesian(cartesian), _laser(laser)
{
}

void AutohomeCommand::execute()
{
    StepperMotorPtr stepper = _cartesian.getStepperMotor(_currentAxis);
    LimitSwitchPtr sw = _cartesian.getLimitSwitch(_currentAxis);

    if (!stepper || !sw)
    {
        _currentStatus = Status::ERROR;
        return;
    }

    switch (_currentState)
    {
    case PRESS:
        stepper->run();
        sw->tick();

        if (sw->wasPressed() || sw->isPressed())
        {
            stepper->setCurrentPosition(0);

            _cartesian.setTargetPosition(_currentAxis, 400);
            _currentState = AutohomeState::RELEASE;

            stepper->pause(500);
        }

        break;

    case RELEASE:
        stepper->run();
        sw->tick();

        if (sw->wasReleased())
        {
            stepper->setCurrentPosition(0);

            _cartesian.setTargetPosition(_currentAxis, -400);
            _currentState = AutohomeState::RETURN;

            stepper->pause(500);
        }

        break;

    case RETURN:
        stepper->run();
        sw->tick();

        if (sw->wasPressed() || sw->isPressed())
        {
            stepper->setCurrentPosition(0);

            if (_currentAxis == Axis::Z)
            {
                _currentStatus = Status::COMPLETED;
                return;
            }

            _currentAxis++;
            _cartesian.setTargetPosition(_currentAxis, -400);
            _currentState = AutohomeState::PRESS;
        }

        break;

    default:
        break;
    }
}

void AutohomeCommand::finish()
{
    for (uint8_t i = 0; i < AXES; i++)
    {
        Axis axis = static_cast<Axis>(i);

        _cartesian.setTargetPosition(axis, 0);
    }
}

void AutohomeCommand::start()
{
    _laser.turnOff();

    _currentAxis = Axis::X;
    _currentState = AutohomeState::PRESS;
    _currentStatus = Status::CONTINUE;

    _cartesian.setTargetPosition(_currentAxis, -400);
}

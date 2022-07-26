#include <Cartesian.h>
#include <Command.h>

AutohomeCommand::AutohomeCommand(Cartesian &cartesian) : _cartesian(cartesian)
{
}

void AutohomeCommand::execute()
{
    StepperMotor *stepper = _cartesian.getStepperMotor(_currentAxis);
    LimitSwitch *sw = _cartesian.getLimitSwitch(_currentAxis);

    if (!stepper || !sw)
    {
        _currentStatus = CommandStatus::ERROR;
        return;
    }

    switch (_currentState)
    {
    case PRESS:
        _cartesian.run();

        if (sw->wasPressed() || sw->isPressed())
        {
            stepper->setCurrentPosition(0);

            _cartesian.moveToLimit(_currentAxis, Direction::POSITIVE);
            _currentState = AutohomeState::RELEASE;

            stepper->pause(500);
        }

        break;

    case RELEASE:
        _cartesian.run();

        if (sw->wasReleased())
        {
            stepper->setCurrentPosition(0);

            _cartesian.moveToLimit(_currentAxis, Direction::NEGATIVE);
            _currentState = AutohomeState::RETURN;

            stepper->pause(500);
        }

        break;

    case RETURN:
        _cartesian.run();

        if (sw->wasPressed() || sw->isPressed())
        {
            stepper->setCurrentPosition(0);

            if (_currentAxis == Axis::Z)
            {
                _currentStatus = CommandStatus::COMPLETED;
                return;
            }

            // move next motor
            _currentAxis++;
            _cartesian.moveToLimit(_currentAxis, Direction::NEGATIVE);
            _currentState = AutohomeState::PRESS;
        }

        break;

    default:
        break;
    }
}

void AutohomeCommand::start()
{
    _cartesian.moveToLimit(_currentAxis, Direction::NEGATIVE);

    _currentAxis = Axis::X;
    _currentState = AutohomeState::PRESS;
    _currentStatus = CommandStatus::CONTINUE;
}

CommandStatus AutohomeCommand::status()
{
    return _currentStatus;
}

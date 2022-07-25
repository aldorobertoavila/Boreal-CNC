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

    float dimensions = _cartesian.getDimensions(_currentAxis);

    switch (_currentState)
    {
    case PRESS:
        _cartesian.run();

        if (sw->wasPressed() || sw->isPressed())
        {
            _cartesian.moveTo(_currentAxis, dimensions);
            _currentState = AutohomeState::RELEASE;
        }

        break;

    case RELEASE:
        _cartesian.run();

        if (sw->wasReleased())
        {
            _cartesian.moveTo(_currentAxis, -dimensions);
            _currentState = AutohomeState::RETURN;
        }

        break;

    case RETURN:
        _cartesian.run();

        if (sw->wasPressed() || sw->isPressed())
        {
            if (_currentAxis == Axis::Z)
            {
                _currentStatus = CommandStatus::COMPLETED;
                stepper->setCurrentPosition(0);
                return;
            }

            _cartesian.moveTo(_currentAxis++, -dimensions);
            _currentState = AutohomeState::RELEASE;
        }

        break;

    default:
        break;
    }
}

void AutohomeCommand::start()
{
    _currentAxis = Axis::X;
    _currentState = AutohomeState::PRESS;
    _currentStatus = CommandStatus::CONTINUE;

    float dimensions = _cartesian.getDimensions(_currentAxis);

    _cartesian.moveTo(_currentAxis, -dimensions);
}

CommandStatus AutohomeCommand::status()
{
    return _currentStatus;
}

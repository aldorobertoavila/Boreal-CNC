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

    float dimension = _cartesian.getDimension(_currentAxis);

    switch (_currentState)
    {
    case PRESS:
        _cartesian.run();

        if (sw->wasPressed() || sw->isPressed())
        {
            stepper->setCurrentPosition(0);

            _cartesian.moveTo(_currentAxis++, Unit::MILLIMETER, -dimension);
            _currentState = AutohomeState::RELEASE;

            stepper->pause(1000);
        }

        break;

    case RELEASE:
        _cartesian.run();

        if (sw->wasReleased())
        {
            _cartesian.moveTo(_currentAxis++, Unit::MILLIMETER, -dimension);
            _currentState = AutohomeState::RETURN;

            stepper->pause(1000);
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

            _cartesian.moveTo(_currentAxis++, Unit::MILLIMETER, -dimension);
            _currentState = AutohomeState::PRESS;
        }

        break;

    default:
        break;
    }
}

void AutohomeCommand::start()
{
    float dimension = _cartesian.getDimension(_currentAxis);

    _cartesian.moveTo(_currentAxis, -dimension);

    _currentAxis = Axis::X;
    _currentState = AutohomeState::PRESS;
    _currentStatus = CommandStatus::CONTINUE;
}

CommandStatus AutohomeCommand::status()
{
    return _currentStatus;
}

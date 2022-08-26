#include <Command.h>

AutohomeCommand::AutohomeCommand(Cartesian &cartesian, Laser &laser) : _cartesian(cartesian), _laser(laser), MoveCommand(cartesian, laser, 0, 0)
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

    float dimension = _cartesian.getDimension(_currentAxis);

    switch (_currentState)
    {
    case AutohomeState::PRESS:
        stepper->run();
        sw->tick();

        if (sw->wasPressed() || sw->isPressed())
        {
            stepper->setCurrentPosition(0);

            _cartesian.setTargetPosition(_currentAxis, Unit::MILLIMETER, dimension);
            _currentState = AutohomeState::RELEASE;

            stepper->pause(500);
        }

        break;

    case AutohomeState::RELEASE:
        stepper->run();
        sw->tick();

        if (sw->wasReleased())
        {
            stepper->setCurrentPosition(0);

            _cartesian.setTargetPosition(_currentAxis, Unit::MILLIMETER, -dimension);
            _currentState = AutohomeState::RETURN;

            stepper->pause(500);
        }

        break;

    case AutohomeState::RETURN:
        stepper->run();
        sw->tick();

        if (sw->wasPressed() || sw->isPressed())
        {
            float homeOffset = _cartesian.getHomeOffset(_currentAxis);

            stepper->setCurrentPosition(0);

            _cartesian.setTargetPosition(_currentAxis, Unit::MILLIMETER, homeOffset);
            _currentState = AutohomeState::OFFSET;

            stepper->pause(500);
        }

        break;

    case AutohomeState::OFFSET:
        stepper->run();
        sw->tick();

        if (stepper->distanceTo() == 0)
        {
            if (_currentAxis == Axis::Z)
            {
                complete();
                return;
            }

            _currentAxis++;
            _cartesian.setTargetPosition(_currentAxis, Unit::MILLIMETER, -dimension);
            _currentState = AutohomeState::PRESS;
        }

        break;

    default:
        break;
    }
}

void AutohomeCommand::setup()
{
    long dimension = _cartesian.getDimension(_currentAxis);

    _laser.turnOff();

    _currentAxis = Axis::X;
    _currentState = AutohomeState::PRESS;
    _currentStatus = Status::CONTINUE;

    _cartesian.setTargetPosition(_currentAxis, Unit::MILLIMETER, -dimension);
}

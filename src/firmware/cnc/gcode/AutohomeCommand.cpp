#include <Command.h>

AutohomeCommand::AutohomeCommand(Cartesian &cartesian, Laser &laser) : _cartesian(cartesian), _laser(laser)
{
}

bool AutohomeCommand::continues()
{
    /*
    Axis axis = _cartesian.getCurrentAxis();

    if (axis == Axis::Z && _currentState == AutohomeState::OFFSET)
    {
        StepperMotorPtr stepper = _cartesian.getStepperMotor(axis);

        if (stepper)
        {
            return stepper->distanceTo() != 0;
        }

        return false;
    }

    return true;
    */
    return false;
}

void AutohomeCommand::execute()
{
    /*
    Axis axis = _cartesian.getCurrentAxis();

    StepperMotorPtr stepper = _cartesian.getStepperMotor(axis);
    LimitSwitchPtr sw = _cartesian.getLimitSwitch(axis);

    if (!stepper || !sw)
    {
        return;
    }

    float dimension = _cartesian.getDimension(axis);

    switch (_currentState)
    {
    case AutohomeState::PRESS:
        stepper->run();
        sw->tick();

        if (sw->wasPressed() || sw->isPressed())
        {
            stepper->setCurrentPosition(0);

            _cartesian.setTargetPosition(axis, Unit::MILLIMETER, dimension);
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

            _cartesian.setTargetPosition(axis, Unit::MILLIMETER, -dimension);
            _currentState = AutohomeState::RETURN;

            stepper->pause(500);
        }

        break;

    case AutohomeState::RETURN:
        stepper->run();
        sw->tick();

        if (sw->wasPressed() || sw->isPressed())
        {
            float homeOffset = _cartesian.getHomeOffset(axis);

            stepper->setCurrentPosition(0);

            _cartesian.setTargetPosition(axis, Unit::MILLIMETER, homeOffset);
            _currentState = AutohomeState::OFFSET;

            stepper->pause(500);
        }

        break;

    case AutohomeState::OFFSET:
        stepper->run();
        sw->tick();

        if (stepper->distanceTo() == 0)
        {
            if (axis == Axis::Z)
            {
                return;
            }

            axis++;
            _cartesian.setCurrentAxis(axis);
            _cartesian.setTargetPosition(axis, Unit::MILLIMETER, -dimension);
            _currentState = AutohomeState::PRESS;
        }

        break;

    default:
        break;
    }
    */
}

void AutohomeCommand::setup()
{
    /*
    _cartesian.setCurrentAxis(Axis::X);
    _laser.turnOff();

    _currentState = AutohomeState::PRESS;

    long dimension = _cartesian.getDimension(Axis::X);

    _cartesian.setTargetPosition(Axis::X, Unit::MILLIMETER, -dimension);
    */
}

void AutohomeCommand::stop()
{
}
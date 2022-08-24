#include <StepperMotor.h>

StepperMotor::StepperMotor(MotorInterface &motorInterface) : _motorInterface(motorInterface)
{
    this->_motorInterface = motorInterface;
    this->_maxSpeed = 1.0;
    this->_minStepInterval = 1.0;
    this->_pauseInterval = 0;
    this->_pauseStartTime = 0;

    enable();
    wakeUp();

    setCurrentPosition(0);
}

void StepperMotor::computeSpeed()
{
    if (_acceleration > 0)
    {
        long deltaDistance = distanceTo();

        if (deltaDistance == 0)
        {
            _speed = 0;
            _stepInterval = 0;
            _nSteps = 0;
            return;
        }

        long distance = (long)(_speed * _speed / (2.0 * _acceleration));

        if (deltaDistance > 0)
        {
            if (_nSteps > 0 && distance >= deltaDistance || _rotation == COUNTERCLOCKWISE)
            {
                _nSteps = -distance;
            }
            else if (_nSteps < 0 && distance < deltaDistance && _rotation == CLOCKWISE)
            {
                _nSteps = -_nSteps;
            }
        }
        else if (deltaDistance < 0)
        {
            if (_nSteps > 0 && distance >= -deltaDistance || _rotation == CLOCKWISE)
            {
                _nSteps = -distance;
            }
            else if (_nSteps < 0 && distance < -deltaDistance && _rotation == COUNTERCLOCKWISE)
            {
                _nSteps = -_nSteps;
            }
        }

        if (_nSteps == 0)
        {
            _accelStepInterval = _initialAccelInterval;
            _rotation = deltaDistance > 0 ? CLOCKWISE : COUNTERCLOCKWISE;
        }
        else
        {
            _accelStepInterval = _accelStepInterval - (2.0 * _accelStepInterval / (4.0 * _nSteps + 1));
            _accelStepInterval = max(_accelStepInterval, _minStepInterval);
        }

        _nSteps++;
        _speed = 1e6 / _accelStepInterval;
        _stepInterval = _accelStepInterval;
    }

    if (_rotation == COUNTERCLOCKWISE)
    {
        _speed = -_speed;
    }
}

void StepperMotor::disable()
{
    _motorInterface.setEnable(HIGH);
    _enable = false;
}

void StepperMotor::enable()
{
    _motorInterface.setEnable(LOW);
    _enable = true;
}

long StepperMotor::distanceTo()
{
    return _targetPos - _currentPos;
}

bool StepperMotor::isEnable()
{
    return _enable;
}

bool StepperMotor::isSleeping()
{
    return _sleep;
}

float StepperMotor::getAcceleration()
{
    return _acceleration;
}

long StepperMotor::getCurrentPosition()
{
    return _currentPos;
}

float StepperMotor::getMaxSpeed()
{
    return _maxSpeed;
}

float StepperMotor::getSpeed()
{
    return _speed;
}

long StepperMotor::getTargetPosition()
{
    return _targetPos;
}

void StepperMotor::move(long relative)
{
    moveTo(_currentPos + relative);
}

void StepperMotor::moveTo(long absolute)
{
    if (_targetPos != absolute)
    {
        _targetPos = absolute;
        computeSpeed();
    }
}

void StepperMotor::pause(unsigned long ms)
{
    _pauseInterval = ms;
    _pauseStartTime = millis();
}

void StepperMotor::reset()
{
    _motorInterface.setReset(LOW);
    delay(RESET_DELAY);
    _motorInterface.setReset(HIGH);
    setCurrentPosition(0);
}

bool StepperMotor::run()
{
    if (runSpeed())
    {
        computeSpeed();
        return true;
    }

    return false;
}

bool StepperMotor::runSpeed()
{

    if (!_enable || _sleep || !_stepInterval || distanceTo() == 0)
    {
        return false;
    }

    if (_pauseInterval > 0)
    {
        unsigned long currentMillis = millis();

        if (currentMillis - _pauseStartTime <= _pauseInterval)
        {
            return false;
        }
        else
        {
            _pauseInterval = 0;
            _pauseStartTime = 0;
        }
    }

    unsigned long currentMicros = micros();

    if (currentMicros - _prevStepTime >= _stepInterval)
    {
        if (_rotation == CLOCKWISE)
        {
            _currentPos++;
        }
        else
        {
            _currentPos--;
        }
        step();

        _prevStepTime = currentMicros;
        return true;
    }

    return false;
}

void StepperMotor::setAcceleration(float acceleration)
{
    if (acceleration < 0)
        acceleration = -acceleration;

    if (acceleration == 0)
    {
        _initialAccelInterval = 0;
        _acceleration = 0;
    }
    else if (_acceleration != acceleration)
    {
        _nSteps = _nSteps * (_acceleration / acceleration);
        _initialAccelInterval = 0.676 * sqrt(2.0 / acceleration) * 1e6;
        _acceleration = acceleration;
        computeSpeed();
    }
}

void StepperMotor::setCurrentPosition(long position)
{
    _currentPos = position;
    _targetPos = position;

    _accelStepInterval = 0;
    _nSteps = 0;
    _speed = 0;
    _stepInterval = 0;
}

void StepperMotor::setMaxSpeed(float maxSpeed)
{
    if (maxSpeed < 0)
        maxSpeed = -maxSpeed;

    if (_maxSpeed != maxSpeed)
    {
        _maxSpeed = maxSpeed;
        _minStepInterval = 1e6 / maxSpeed;

        if (_nSteps > 0)
        {
            _nSteps = (long)((_speed * _speed) / (2.0 * _acceleration));
            computeSpeed();
        }
    }
}

void StepperMotor::setResolution(Resolution res)
{
    _motorInterface.setResolution(res);
    _resolution = res;
}

void StepperMotor::setSpeed(float speed)
{
    if (speed == _speed)
        return;

    speed = constrain(speed, -_maxSpeed, _maxSpeed);

    if (speed == 0)
    {
        _stepInterval = 0;
    }
    else
    {
        _stepInterval = fabs(1e6 / speed);
        _rotation = speed > 0 ? CLOCKWISE : COUNTERCLOCKWISE;
    }

    _speed = speed;
}

void StepperMotor::step()
{
    _motorInterface.setRotation(_rotation);
    _motorInterface.step();
}

void StepperMotor::sleep()
{
    _motorInterface.setSleep(LOW);
    _sleep = true;

    delay(SLEEP_DELAY);
}

void StepperMotor::wakeUp()
{
    _motorInterface.setSleep(HIGH);
    _sleep = false;

    delay(SLEEP_DELAY);
}
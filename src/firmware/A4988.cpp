#include <A4988.h>

PinOutMotorInterface::PinOutMotorInterface(uint8_t enaPin, uint8_t dirPin, uint8_t stepPin, uint8_t ms1Pin, uint8_t ms2Pin, uint8_t ms3Pin)
{
    this->_enaPin = enaPin;
    this->_dirPin = dirPin;
    this->_stepPin = stepPin;
    this->_ms1Pin = ms1Pin;
    this->_ms2Pin = ms2Pin;
    this->_ms3Pin = ms3Pin;
}

void PinOutMotorInterface::enable()
{
    digitalWrite(_enaPin, LOW);
}

void PinOutMotorInterface::disable()
{
    digitalWrite(_enaPin, HIGH);
}

void PinOutMotorInterface::resolution(Resolution res)
{
    switch(res)
    {
        case FULL:
            digitalWrite(_ms1Pin, LOW);
            digitalWrite(_ms2Pin, LOW);
            digitalWrite(_ms3Pin, LOW);
            break;
        case HALF:
            digitalWrite(_ms1Pin, HIGH);
            digitalWrite(_ms2Pin, LOW);
            digitalWrite(_ms3Pin, LOW);
            break;
        case QUARTER:
            digitalWrite(_ms1Pin, LOW);
            digitalWrite(_ms2Pin, HIGH);
            digitalWrite(_ms3Pin, LOW);
            break;
        case EIGHTH:
            digitalWrite(_ms1Pin, HIGH);
            digitalWrite(_ms2Pin, HIGH);
            digitalWrite(_ms3Pin, LOW);
            break;
        case SIXTEENTH:
            digitalWrite(_ms1Pin, HIGH);
            digitalWrite(_ms2Pin, HIGH);
            digitalWrite(_ms3Pin, HIGH);
            break;
        default:
            break;
    }
}

void PinOutMotorInterface::step(Direction dir, unsigned long pulseWidth)
{
    digitalWrite(_dirPin, dir);
    digitalWrite(_stepPin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(_stepPin, LOW);
}

A4988::A4988(MotorInterface &motorInterface) : _motorInterface(motorInterface)
{
    this->_motorInterface = motorInterface;
    this->_maxSpeed = 1.0;
    this->_minStepInterval = 1.0;
}

void A4988::computeSpeed()
{
    if(_acceleration > 0)
    {
        long deltaDistance = distanceTo();
        long distance = (long) (_speed * _speed / (2.0 * _acceleration));

        if (deltaDistance == 0 && distance < 0)
        {
            _speed = 0;
            _stepInterval = 0;
            _nSteps = 0;
            return;
        }

        if (deltaDistance > 0)
        {
            if (_nSteps > 0 && distance >= deltaDistance || _direction == DIRECTION_CCW)
            {
                _nSteps = -distance;
            }
            else if (_nSteps < 0 && distance < deltaDistance && _direction == DIRECTION_CW)
            {
                _nSteps = -distance;
            }
        }
        else if (deltaDistance < 0)
        {
            if (_nSteps > 0 && distance >= -deltaDistance || _direction == DIRECTION_CW)
            {
                _nSteps = -distance;
            }
            else if (_nSteps < 0 && distance < -deltaDistance && _direction == DIRECTION_CCW)
            {
                _nSteps = -distance;
            }
        }

        if (_nSteps == 0)
        {
            _accelStepInterval = _initialAccelInterval;
            _direction = deltaDistance > 0 ? DIRECTION_CW : DIRECTION_CCW;
        }
        else
        {
            _accelStepInterval = _accelStepInterval - (2.0 * _accelStepInterval / (4.0 * _nSteps + 1));
            _accelStepInterval = max(_accelStepInterval, _minStepInterval);
        }

        _nSteps++;
        _speed = 1e6 / _accelStepInterval;
        _stepInterval = _accelStepInterval;

        if (_direction == DIRECTION_CCW)
            _speed = -_speed;
    }
}

void A4988::disable()
{
    _motorInterface.disable();
}

long A4988::distanceTo()
{
    return _targetPos - _currentPos;
}

void A4988::enable()
{
    _motorInterface.enable();
}

float A4988::getAcceleration()
{
    return _acceleration;
}

long A4988::getCurrentPosition()
{
    return _currentPos;
}

float A4988::getMaxSpeed()
{
    return _maxSpeed;
}

float A4988::getSpeed()
{
    return _speed;
}

long A4988::getTargetPosition()
{
    return _targetPos;
}

void A4988::move(long relative)
{
    moveTo(_currentPos + relative);
}

void A4988::moveTo(long absolute)
{
    if (_targetPos != absolute)
    {
        _targetPos = absolute;
        computeSpeed();
    }
}

bool A4988::run()
{
        Serial.println("Step");

    if(runSpeed())
    {
        computeSpeed();
        return true;
    }

    return false;
}

bool A4988::runSpeed()
{
    if(!_stepInterval || distanceTo() < 0)
        return false;

    unsigned long currentTime = micros();

    if (currentTime - _lastStepTime >= _stepInterval)
    {
        if (_direction == DIRECTION_CW)
        {
            _currentPos++;
        }
        else
        {
            _currentPos--;
        }
        step();

        _lastStepTime = currentTime;

        return true;   
    }

    return false;
}

void A4988::setAcceleration(float acceleration)
{
    if(acceleration < 0)
        acceleration = -acceleration;

    if (_acceleration != acceleration)
    {
        _initialAccelInterval = sqrt(2.0 / acceleration) * 1e6;
        _acceleration = acceleration;
    }

}

void A4988::setCurrentPosition(long position)
{
    _currentPos = position;
    _targetPos = position;
    _nSteps = 0;
    _speed = 0;
    _stepInterval = 0;
}

void A4988::setMaxSpeed(float maxSpeed)
{
    if(maxSpeed < 0)
        maxSpeed = -maxSpeed;
	
    if (_maxSpeed != maxSpeed)
    {
        _maxSpeed = maxSpeed;
        _minStepInterval = 1e6 / maxSpeed;
    }
}

void A4988::setResolution(Resolution res)
{
    _motorInterface.resolution(res);
    _resolution = res;
}

void A4988::setSpeed(float speed)
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
        _direction = speed > 0 ? DIRECTION_CW : DIRECTION_CCW;
    }

    _speed = speed;
}

void A4988::step()
{
    _motorInterface.step(_direction, MIN_PULSE_WIDTH);
}
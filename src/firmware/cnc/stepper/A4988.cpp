#include <A4988.h>

// Emerging from Reset delay (ms)
#define RESET_DELAY 1

// PWM per Step delay (us)
#define STEP_DELAY 10

// Emerging from Sleep delay (ms)
#define SLEEP_DELAY 1

// Parallel Data Outputs

// Q0
#define DIR_OUT 0
// Q1
#define STEP_OUT 1
// Q2
#define SLEEP_OUT 2
// Q3
#define RESET_OUT 3
// Q4
#define MS3_OUT 4
// Q5
#define MS2_OUT 5
// Q6
#define MS1_OUT 6
// Q7
#define ENABLE_OUT 7

ShiftRegisterMotorInterface::ShiftRegisterMotorInterface(SPIClass &spi, uint8_t csPin, long spiClk) : _spi(spi)
{
    this->_spiClk = spiClk;
    this->_csPin = csPin;
    this->_spi = spi;

    bitWrite(_dataIn, ENABLE_OUT, LOW);
    bitWrite(_dataIn, RESET_OUT, HIGH);
    bitWrite(_dataIn, SLEEP_OUT, HIGH);
    updateShiftOut();
}

void ShiftRegisterMotorInterface::setEnable(bool enable)
{
    bitWrite(_dataIn, ENABLE_OUT, enable);
    updateShiftOut();
}

void ShiftRegisterMotorInterface::setReset(bool reset)
{
    bitWrite(_dataIn, RESET_OUT, reset);
    updateShiftOut();
}

void ShiftRegisterMotorInterface::setResolution(Resolution res)
{
    switch (res)
    {
    case FULL:
        bitWrite(_dataIn, MS1_OUT, LOW);
        bitWrite(_dataIn, MS2_OUT, LOW);
        bitWrite(_dataIn, MS3_OUT, LOW);
        updateShiftOut();
        break;
    case HALF:
        bitWrite(_dataIn, MS1_OUT, HIGH);
        bitWrite(_dataIn, MS2_OUT, LOW);
        bitWrite(_dataIn, MS3_OUT, LOW);
        updateShiftOut();
        break;
    case QUARTER:
        bitWrite(_dataIn, MS1_OUT, LOW);
        bitWrite(_dataIn, MS2_OUT, HIGH);
        bitWrite(_dataIn, MS3_OUT, LOW);
        updateShiftOut();
        break;
    case EIGHTH:
        bitWrite(_dataIn, MS1_OUT, HIGH);
        bitWrite(_dataIn, MS2_OUT, HIGH);
        bitWrite(_dataIn, MS3_OUT, LOW);
        updateShiftOut();
        break;
    case SIXTEENTH:
        bitWrite(_dataIn, MS1_OUT, HIGH);
        bitWrite(_dataIn, MS2_OUT, HIGH);
        bitWrite(_dataIn, MS3_OUT, HIGH);
        updateShiftOut();
        break;
    default:
        break;
    }
}

void ShiftRegisterMotorInterface::setSleep(bool sleep)
{
    bitWrite(_dataIn, SLEEP_OUT, sleep);
    updateShiftOut();
    delay(SLEEP_DELAY);
}

void ShiftRegisterMotorInterface::step(Rotation rot)
{
    bitWrite(_dataIn, DIR_OUT, rot);
    bitWrite(_dataIn, STEP_OUT, HIGH);
    updateShiftOut();
    delayMicroseconds(STEP_DELAY);
    bitWrite(_dataIn, STEP_OUT, LOW);
    updateShiftOut();
}

void ShiftRegisterMotorInterface::updateShiftOut()
{
    _spi.beginTransaction(SPISettings(_spiClk, MSBFIRST, SPI_MODE0));
    digitalWrite(_csPin, LOW);
    _spi.transfer(_dataIn);
    digitalWrite(_csPin, HIGH);
    _spi.endTransaction();
}

A4988::A4988(MotorInterface &motorInterface) : _motorInterface(motorInterface)
{
    this->_motorInterface = motorInterface;
    this->_maxSpeed = 1.0;
    this->_minStepInterval = 1.0;

    enable();
    wakeUp();
}

void A4988::computeSpeed()
{
    if(_acceleration > 0)
    {
        long deltaDistance = distanceTo();

        if (deltaDistance == 0)
        {
            _speed = 0;
            _stepInterval = 0;
            _nSteps = 0;
            return;
        }

        long distance = (long) (_speed * _speed / (2.0 * _acceleration));

        if (deltaDistance > 0)
        {
            if (_nSteps > 0 && distance >= deltaDistance || _direction == COUNTERCLOCKWISE)
            {
                _nSteps = -distance;
            }
            else if (_nSteps < 0 && distance < deltaDistance && _direction == CLOCKWISE)
            {
                _nSteps = -_nSteps;
            }
        }
        else if (deltaDistance < 0)
        {
            if (_nSteps > 0 && distance >= -deltaDistance || _direction == CLOCKWISE)
            {
                _nSteps = -distance;
            }
            else if (_nSteps < 0 && distance < -deltaDistance && _direction == COUNTERCLOCKWISE)
            {
                _nSteps = -_nSteps;
            }
        }

        if (_nSteps == 0)
        {
            _accelStepInterval = _initialAccelInterval;
            _direction = deltaDistance > 0 ? CLOCKWISE : COUNTERCLOCKWISE;
        }
        else
        {
            _accelStepInterval = _accelStepInterval - (2.0 * _accelStepInterval / (4.0 * _nSteps + 1));
            _accelStepInterval = max(_accelStepInterval, _minStepInterval);
        }

        _nSteps++;
        _speed = 1e6 / _accelStepInterval;
        _stepInterval = _accelStepInterval;

        if (_direction == COUNTERCLOCKWISE)
          _speed = -_speed;
    }
}

void A4988::disable()
{
    _motorInterface.setEnable(HIGH);
    _enable = false;
}

void A4988::enable()
{
    _motorInterface.setEnable(LOW);
    _enable = true;
}

long A4988::distanceTo()
{
    return _targetPos - _currentPos;
}

bool A4988::isEnable()
{
    return _enable;
}

bool A4988::isSleeping()
{
    return _sleep;
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

void A4988::reset()
{
    _motorInterface.setReset(LOW);
    delay(RESET_DELAY);
    _motorInterface.setReset(HIGH);
    setCurrentPosition(0);
}

bool A4988::run()
{
    if(runSpeed())
    {
        computeSpeed();
        return true;
    }

    return false;
}

bool A4988::runSpeed()
{
    if(!_enable || _sleep || !_stepInterval || distanceTo() == 0)
        return false;

    unsigned long currentTime = micros();

    if (currentTime - _prevStepTime >= _stepInterval)
    {
        if (_direction == CLOCKWISE)
        {
            _currentPos++;
        }
        else
        {
            _currentPos--;
        }
        step();

        _prevStepTime = currentTime;

        return true;   
    }

    return false;
}


void A4988::setAcceleration(float acceleration)
{
    if(acceleration < 0)
        acceleration = -acceleration;

    if(acceleration == 0)
    {
        _initialAccelInterval = 0;
        _acceleration = 0;
    }
    else if(_acceleration != acceleration)
    {
        _nSteps = _nSteps * (_acceleration / acceleration);
        _initialAccelInterval = 0.676 * sqrt(2.0 / acceleration) * 1e6;
        _acceleration = acceleration;
        computeSpeed();
    }

}

void A4988::setCurrentPosition(long position)
{
    _currentPos = position;
    _targetPos = position;

    _accelStepInterval = 0;
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

        if (_nSteps > 0)
      {
          _nSteps = (long)((_speed * _speed) / (2.0 * _acceleration));
          computeSpeed();
      }
    }
}

void A4988::setResolution(Resolution res)
{
    _motorInterface.setResolution(res);
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
        _direction = speed > 0 ? CLOCKWISE : COUNTERCLOCKWISE;
    }

    _speed = speed;
}

void A4988::step()
{
    _motorInterface.step(_direction);
}

void A4988::sleep()
{
    _motorInterface.setSleep(LOW);
    _sleep = true;

    delay(SLEEP_DELAY);
}

void A4988::wakeUp()
{
    _motorInterface.setSleep(HIGH);
    _sleep = false;

    delay(SLEEP_DELAY);
}

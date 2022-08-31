#include <Command.h>

DwellCommand::DwellCommand(unsigned long remainTime)
{
    this->_remainTime = remainTime;
}

bool DwellCommand::continues()
{
    return _remainTime > 0 ? _timeElapsed < _remainTime : false;
}

void DwellCommand::execute()
{
    _timeElapsed = millis() - _startTime;
}

void DwellCommand::setup()
{
    _startTime = millis();
}

void DwellCommand::stop()
{
    if(_timeElapsed >= _remainTime)
    {
        _remainTime = 0; 
    }
    else
    {
        _remainTime -= _timeElapsed;
    }
}
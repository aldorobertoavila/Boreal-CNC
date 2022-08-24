#include <Command.h>

DwellCommand::DwellCommand(unsigned long remainTime)
{
    this->_remainTime = remainTime;
}

void DwellCommand::execute()
{
    if (millis() - _startTime > _remainTime)
    {
        complete();
    }
}

void DwellCommand::complete()
{
    _remainTime = 0;
    _currentStatus = Status::COMPLETED;
}

void DwellCommand::setup()
{
    _startTime = millis();
    _currentStatus = Status::CONTINUE;
}

void DwellCommand::stop()
{
    _remainTime = 0;

    _currentStatus = Status::STOPPED;
}
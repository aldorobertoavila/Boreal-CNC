#include <Command.h>

DwellCommand::DwellCommand(unsigned long remainTime)
{
    this->_remainTime = remainTime;
}

bool DwellCommand::continues()
{
    return millis() - _startTime > _remainTime;
}

void DwellCommand::execute()
{
}

void DwellCommand::setup()
{
    _startTime = millis();
}

void DwellCommand::stop()
{
    _remainTime = 0;
}
#include <Command.h>

DwellCommand::DwellCommand(unsigned long remainTime)
{
    this->_remainTime = remainTime;
}

void DwellCommand::execute()
{
    if(millis() - _startTime > _remainTime)
    {
        _currentStatus = Status::COMPLETED;        
    }
}

void DwellCommand::finish()
{

}

void DwellCommand::start()
{
    _startTime = millis();
    _currentStatus = Status::CONTINUE;
}

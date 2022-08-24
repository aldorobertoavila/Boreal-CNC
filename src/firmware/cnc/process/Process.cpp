#include <Process.h>

Process::Process(fs::FS &fs, RTC &rtc, String path, String filename) : _fs(fs), _rtc(rtc)
{
    this->_filename = filename;
    this->_path = path;
}

uint8_t Process::getPreviousProgress()
{
    return _previousProgress;
}

String Process::getPreviousTime()
{
    return _previousTime;
}

uint8_t Process::getProgress()
{
    if (_file)
    {
        _previousProgress = _progress;
        _progress = map(_file.position(), 0, _file.size(), 0, 100);
    }

    return _progress;
}

String Process::getTime()
{
    if (_file)
    {
        _previousTime = _time;
        _time = _rtc.getTime("%H:%M");
    }

    return _time;
}

String Process::getName()
{
    return _filename;
}

String Process::readNextLine()
{
    if (_status != Status::CONTINUE || !_file)
    {
        return "";
    }

    if (!_file.available())
    {
        _status = Status::COMPLETED;
        return "";
    }

    String line = _file.readStringUntil('\n');
    int comment = line.indexOf(";");

    if (comment == 0)
    {
        return "";
    }

    if (comment > 0)
    {
        line = line.substring(0, comment);
    }

    return line;
}

void Process::pause()
{
    if (_status == Status::CONTINUE)
    {
        _status = Status::PAUSED;
    }
}

void Process::resume()
{
    if (_status == Status::PAUSED)
    {
        _status = Status::CONTINUE;
    }
}

void Process::setup()
{
    _rtc.setTime();
    _file = _fs.open(_path + _filename);
    _status = Status::CONTINUE;
}

Status Process::status()
{
    return _status;
}

void Process::stop()
{
    if (_status == Status::CONTINUE || _status == Status::PAUSED)
    {
        if (_file)
        {
            _file.close();
        }

        _status = Status::STOPPED;
    }
}
#include <Process.h>

Process::Process(fs::FS &fs, String path) : _fs(fs)
{
    this->_path = path;
}

String Process::readNextLine()
{
    if (_file)
    {
        if (!_file.available())
        {
            _status = Status::COMPLETED;
        }

        return _file.readStringUntil('\n');
    }

    return "";
}

void Process::pause()
{
    if (_status == Status::CONTINUE)
    {
        if (_file)
        {
            _file.close();
        }

        _status = Status::PAUSED;
    }
}

void Process::resume()
{
    if (_status == Status::PAUSED)
    {
        _file = _fs.open(_path);
        _status = Status::CONTINUE;
    }
}

void Process::start()
{
    _file = _fs.open(_path);
    _status = Status::CONTINUE;
}

Status Process::status()
{
    return _status;
}

void Process::stop()
{
    if (_file)
    {
        _file.close();
    }

    _status = Status::STOPPED;
}
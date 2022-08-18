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

uint8_t Process::progress()
{
    if(_file)
    {
        return map(_file.position(), 0, _file.size(), 0, 100);
    }

    return 0;
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
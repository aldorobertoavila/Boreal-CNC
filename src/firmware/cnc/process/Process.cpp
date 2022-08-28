#include <Process.h>

TFProcess::TFProcess(fs::FS &fs, RTC &rtc, Cartesian &cartesian, Laser &laser, const char *path, const char *filename) : _fs(fs), _rtc(rtc), _cartesian(cartesian), _laser(laser)
{
    this->_filename = filename;
    this->_path = path;
}

uint8_t TFProcess::getPrevProgress()
{
    return _prevProgress;
}

tm TFProcess::getPrevTime()
{
    return _prevTime;
}

uint8_t TFProcess::getProgress()
{
    if (_file)
    {
        _prevProgress = _progress;
        _progress = map(_file.position(), 0, _file.size(), 0, 100);
    }

    return _progress;
}

tm TFProcess::getTime()
{
    if (_file)
    {
        _prevTime = _time;
        _time = _rtc.getTimeStruct();
    }

    return _time;
}

const char *TFProcess::getName()
{
    return _filename;
}

float TFProcess::parseNumber(String &line, char arg, float val)
{
    int index = line.indexOf(arg);

    if (index == -1)
    {
        return val;
    }

    line = line.substring(index + 1);
    index = line.indexOf(' ');
    line = line.substring(0, index);

    return line.toInt();
}

void TFProcess::nextCommand(CommandQueue &commands)
{
    if (!_file || _status != Status::CONTINUE)
    {
        return;
    }

    if (commands.size() >= CMD_QUEUE_SIZE)
    {
        return;
    }

    if (!_file.available())
    {
        _status = Status::COMPLETED;
        // TODO push update lcd
        return;
    }

    String line = _file.readStringUntil('\n');

    Serial.println(line);

    if (line.isEmpty())
    {
        return;
    }

    int comment = line.indexOf(";");

    if (comment == 0)
    {
        return;
    }

    if (comment > 0)
    {
        line = line.substring(0, comment);
    }

    int code = parseNumber(line, 'G', -1);

    switch (code)
    {
    case 0:
    case 1:
        commands.push(std::make_shared<LinearMoveCommand>(_cartesian, _laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'F', 0), parseNumber(line, 'S', 0)));
    case 2:
    case 3:
        if (line.indexOf('I') > 0 || line.indexOf('J') > 0)
        {
            if (line.indexOf('R') > 0)
            {
                return;
            }

            commands.push(std::make_shared<ArcMoveCommand>(_cartesian, _laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'I', 0), parseNumber(line, 'J', 0), parseNumber(line, 'K', 0), parseNumber(line, 'F', 0), parseNumber(line, 'S', 0)));
        }
        else
        {
            if (line.indexOf('X') < 0 && line.indexOf('Y') < 0)
            {
                return;
            }

            commands.push(std::make_shared<CircleMoveCommand>(_cartesian, _laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'R', 0), parseNumber(line, 'F', 0), parseNumber(line, 'S', 0)));
        }

        break;
    case 4:
        if (line.indexOf('S') > 0)
        {
            commands.push(std::make_shared<DwellCommand>(parseNumber(line, 'S', 0) * 1000));
        }
        else
        {
            commands.push(std::make_shared<DwellCommand>(parseNumber(line, 'P', 0)));
        }
        break;

    case 28:
        commands.push(std::make_shared<AutohomeCommand>(_cartesian, _laser));
        break;
    default:
        break;
    }
}

void TFProcess::pause()
{
    if (_status == Status::CONTINUE)
    {
        _status = Status::PAUSED;
    }
}

void TFProcess::resume()
{
    if (_status == Status::PAUSED)
    {
        _status = Status::CONTINUE;
    }
}

void TFProcess::setup()
{
    _rtc.setTime();
    _file = _fs.open(_path);
    _status = Status::CONTINUE;
}

Status TFProcess::status()
{
    return _status;
}

void TFProcess::stop()
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
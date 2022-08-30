#include <Process.h>

TFProcess::TFProcess(fs::FS &fs, RTC &rtc, Cartesian &cartesian, Laser &laser, const char *path, const char *filename) : _fs(fs), _rtc(rtc), _cartesian(cartesian), _laser(laser)
{
    this->_filename = filename;
    this->_path = path;

    this->_paused = false;
    this->_stopped = false;
}

bool TFProcess::continues()
{
    return _file && _file.available();
}

uint8_t TFProcess::getPrevProgress()
{
    return _prevProgress;
}

tm TFProcess::getPrevTime()
{
    return _prevTime;
}

const char *TFProcess::getName()
{
    return _filename;
}

uint8_t TFProcess::getProgress()
{
    if (_file)
    {
        _prevProgress = _progress;
        _progress = map(_file.position(), 0, _file.size(), 0, 100); // map position to [0, 100]
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

bool TFProcess::isPaused()
{
    return _paused;
}

bool TFProcess::isStopped()
{
    return _stopped;
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
    if (_stopped || _paused)
    {
        return;
    }

    if (!_file)
    {
        return;
    }

    if (commands.size() >= CMD_QUEUE_SIZE)
    {
        return;
    }

    String line = _file.readStringUntil('\n');

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

    int gcode = parseNumber(line, 'G', -1);
    int mcode = parseNumber(line, 'M', -1);

    if (gcode >= 0 && mcode >= 0)
    {
        return;
    }

    switch (gcode)
    {
    case 0:
    case 1:
        commands.push(std::make_shared<LinearMoveCommand>(_cartesian, _laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'F', 0), parseNumber(line, 'S', 0)));
        break;
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
    };

    switch (mcode)
    {
    case 3:
    case 4:
        if (line.indexOf('I') > 0)
        {
            commands.push(std::make_shared<LaserOnCommand>(_laser, parseNumber(line, 'S', _laser.getMaxPower()), InlineMode::ON));
        }
        else
        {
            commands.push(std::make_shared<LaserOnCommand>(_laser, parseNumber(line, 'S', _laser.getMaxPower()), InlineMode::OFF));
        }
        break;
    case 5:
        commands.push(std::make_shared<LaserOffCommand>(_laser));
        break;

    default:
        break;
    }
}

void TFProcess::pause()
{
    _laser.setInlineMode(InlineMode::OFF);
    _laser.turnOff();
    _paused = true;
}

void TFProcess::setup()
{
    if (_stopped)
    {
        return;
    }

    if(_paused)
    {
        _paused = false;
    }

    if (!_file)
    {
        _rtc.setTime();
        _file = _fs.open(_path);
    }

    _cartesian.enableSteppers();
}

void TFProcess::stop()
{
    if (_file)
    {
        _file.close();
    }

    _cartesian.disableSteppers();

    _laser.setInlineMode(InlineMode::OFF);
    _laser.turnOff();
    _stopped = true;
}
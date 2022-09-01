#pragma once

#include <Command.h>
#include <Rotary.h>

#include <Arduino.h>
#include <ESP32Time.h>
#include <FS.h>

#include <queue>

#define CMD_QUEUE_SIZE 10

using namespace std;

using RTC = ESP32Time;

using CommandPtr = std::shared_ptr<Command>;
using CommandQueue = queue<CommandPtr>;

class Process
{
public:
    virtual uint8_t getPrevProgress()
    {
        return _prevProgress;
    }

    virtual tm getPrevTime()
    {
        return _prevTime;
    }

    virtual tm getTime()
    {
        return _time;
    }

    virtual bool isPaused()
    {
        return _paused;
    }

    virtual bool isStopped() 
    {
        return _stopped;
    }

    virtual bool continues();

    virtual uint8_t getProgress() = 0;

    virtual const char *getName() = 0;

    virtual void nextCommand(CommandQueue &commands) = 0;

    virtual void pause() = 0;

    virtual void setup() = 0;

    virtual void stop() = 0;

protected:
    uint8_t _prevProgress;
    uint8_t _progress;
    tm _prevTime;
    tm _time;
    bool _paused;
    bool _stopped;
};

using ProcessPtr = std::shared_ptr<Process>;
using ProcessQueue = queue<ProcessPtr>;

class TFProcess : public Process
{
public:
    TFProcess(fs::FS &fs, RTC &rtc, Cartesian &cartesian, Laser &laser, const char *path, const char *filename);

    bool continues() override;

    const char *getName() override;

    uint8_t getProgress() override;

    tm getTime() override;

    void nextCommand(CommandQueue &commands) override;

    void pause() override;

    void setup() override;

    void stop() override;

protected:
    float parseNumber(String &line, char arg, float val);

    Cartesian &_cartesian;
    Laser &_laser;
    RTC &_rtc;
    fs::FS &_fs;
    fs::File _file;
    const char *_filename;
    const char *_path;
};

class CommandProcess : public Process
{
public:
    CommandProcess(const char *name, Cartesian &cartesian, RTC &rtc, CommandPtr command);

    bool continues() override;

    const char *getName() override;

    uint8_t getProgress() override;

    tm getTime() override;

    void nextCommand(CommandQueue &commands) override;

    void pause() override{};

    void setup() override;

    void stop() override;

protected:
    Cartesian &_cartesian;
    RTC &_rtc;
    CommandPtr _command;
    const char *_name;
    bool _done;
};
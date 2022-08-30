#pragma once

#include <Command.h>

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
    virtual bool continues();

    virtual uint8_t getPrevProgress() = 0;

    virtual tm getPrevTime() = 0;

    virtual const char *getName() = 0;

    virtual uint8_t getProgress() = 0;

    virtual tm getTime() = 0;

    virtual void nextCommand(CommandQueue &commands) = 0;

    virtual void setup() = 0;

    virtual void stop() = 0;

protected:
    uint8_t _prevProgress;
    uint8_t _progress;
    tm _prevTime;
    tm _time;
};

using ProcessPtr = std::shared_ptr<Process>;
using ProcessQueue = queue<ProcessPtr>;

class TFProcess : public Process
{
public:
    TFProcess(fs::FS &fs, RTC &rtc, Cartesian &cartesian, Laser &laser, const char *path, const char *filename);

    bool continues() override;

    uint8_t getPrevProgress() override;

    tm getPrevTime() override;

    const char *getName() override;

    uint8_t getProgress() override;

    tm getTime() override;

    void nextCommand(CommandQueue &commands) override;

    void setup() override;

    void stop() override;

protected:
    float parseNumber(String &line, char arg, float val);

    Cartesian &_cartesian;
    fs::FS &_fs;
    Laser &_laser;
    RTC &_rtc;
    fs::File _file;
    const char *_filename;
    const char *_path;
    size_t _position;
};
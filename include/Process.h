#pragma once

#include <Command.h>
#include <Status.h>

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
    virtual uint8_t getPrevProgress() = 0;

    virtual tm getPrevTime() = 0;

    virtual uint8_t getProgress() = 0;

    virtual tm getTime() = 0;

    virtual const char *getName() = 0;

    virtual void nextCommand(CommandQueue &commands) = 0;

    virtual void pause() = 0;

    virtual void resume() = 0;

    virtual void setup() = 0;

    virtual Status status() = 0;

    virtual void stop() = 0;

protected:
    tm _prevTime;
    tm _time;
    uint8_t _prevProgress;
    uint8_t _progress;
    Status _status;
};

using ProcessPtr = std::shared_ptr<Process>;
using ProcessQueue = queue<ProcessPtr>;

class TFProcess : public Process
{
public:
    TFProcess(fs::FS &fs, RTC &rtc, Cartesian &cartesian, Laser &laser, const char *path, const char *filename);

    uint8_t getPrevProgress() override;

    tm getPrevTime() override;

    uint8_t getProgress() override;

    tm getTime() override;

    const char *getName() override;

    void nextCommand(CommandQueue &commands) override;

    void pause() override;

    void resume() override;

    void setup() override;

    Status status() override;

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
};
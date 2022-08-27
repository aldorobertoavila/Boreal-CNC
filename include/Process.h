#pragma once

#include <Status.h>

#include <Arduino.h>
#include <FS.h>

#include <ESP32Time.h>

using namespace std;

using RTC = ESP32Time;

class Process
{
public:
    Process(fs::FS &fs, RTC &rtc, const char *path, const char *filename);

    unsigned long getLastStopTime();

    uint8_t getPreviousProgress();

    tm getPreviousTime();

    uint8_t getProgress();

    tm getTime();

    const char *getName();

    const char *readNextLine();

    void pause();

    void resume();

    void setup();

    Status status();

    void stop();

private:
    fs::FS &_fs;
    fs::File _file;
    RTC _rtc;
    Status _status;
    tm _previousTime;
    tm _time;
    const char *_filename;
    const char *_path;
    uint8_t _previousProgress;
    uint8_t _progress;
    unsigned long _lastStopTime;
};
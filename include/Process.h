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
    Process(fs::FS &fs, RTC &rtc, String path, String filename);

    unsigned long getLastStopTime();

    uint8_t getPreviousProgress();

    String getPreviousTime();

    uint8_t getProgress();

    String getTime();

    String getName();

    String readNextLine();

    void pause();

    void resume();

    void setup();

    Status status();

    void stop();

private:
    fs::FS &_fs;
    fs::File _file;
    RTC _rtc;
    String _filename;
    String _path;
    String _previousTime;
    Status _status;
    String _time;
    uint8_t _previousProgress;
    uint8_t _progress;
    unsigned long _lastStopTime;
};
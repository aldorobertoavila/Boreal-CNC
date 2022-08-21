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
    Process(fs::FS &fs, RTC &rtc, String path);

    uint8_t getPreviousProgress();

    String getPreviousTime();

    uint8_t getProgress();

    String getTime();

    String name();

    String readNextLine();

    void pause();

    void resume();

    void start();

    Status status();

    void stop();

private:
    fs::FS &_fs;
    RTC _rtc;
    fs::File _file;
    String _path;
    Status _status;
    uint8_t _previousProgress;
    uint8_t _progress;
    String _previousTime;
    String _time;
};
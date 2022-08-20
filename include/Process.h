#pragma once

#include <Status.h>

#include <Arduino.h>
#include <FS.h>

using namespace std;

class Process
{
public:
    Process(fs::FS &fs, String path);

    uint8_t getPreviousProgress();

    uint8_t getProgress();

    String name();

    String readNextLine();

    void pause();

    void resume();

    void start();

    Status status();

    void stop();

private:
    fs::FS &_fs;
    fs::File _file;
    String _path;
    Status _status;
    uint8_t _progress;
    uint8_t _previousProgress;
};
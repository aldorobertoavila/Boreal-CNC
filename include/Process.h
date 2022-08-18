#pragma once

#include <Status.h>

#include <Arduino.h>
#include <FS.h>

using namespace std;

class Process
{
public:
    Process(fs::FS &fs, String path);

    String readNextLine();

    void pause();

    void resume();

    void start();

    Status status();

    uint8_t progress();

    void stop();

private:
    fs::FS &_fs;
    fs::File _file;
    String _path;
    Status _status;
};
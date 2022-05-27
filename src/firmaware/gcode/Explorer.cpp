#include <Arduino.h>
#include <Explorer.h>

Explorer::Explorer(const char *configFileName, uint8_t maxLineLength) : _configFileName(configFileName), _maxLineLength(maxLineLength)
{
    _configFileName = configFileName;
    _maxLineLength = maxLineLength;
}

void Explorer::initSD(int csPin)
{

}

void Explorer::readFile(const char *path)
{

}
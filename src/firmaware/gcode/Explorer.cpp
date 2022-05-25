#include <Explorer.h>

#include <FS.h>
#include <SD.h>

Explorer::Explorer(const char *configFileName, uint8_t maxLineLength) : _configFileName(configFileName), _maxLineLength(maxLineLength)
{
    _configFileName = configFileName;
    _maxLineLength = maxLineLength;
}

void Explorer::initSD(int csPin)
{
    // TODO spi speed from config file
    SD.begin(csPin, SPI_FULL_SPEED);
}

void Explorer::readFile(const char *path)
{
    File file = SD.open(path);

    if (!file)
    {
        return;
    }

    while (file.available())
    {
        Serial.write(file.read());
    }

    file.close();
}
#include <Arduino.h>

class Explorer
{
public:
    Explorer(const char *configFileName, uint8_t maxLineLength);

    void initSD(int csPin);

    void readFile(const char *path);

private:
    const char *_configFileName;
    uint8_t _maxLineLength;
};
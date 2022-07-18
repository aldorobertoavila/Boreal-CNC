#include <Command.h>
#include <SD.h>

class GCode
{
public:
    GCode(fs::FS fileSystem);

    bool available();

    bool continues();

    Command readAsCommand();

private:
    uint32_t _currentLine;
    File *_file;
};

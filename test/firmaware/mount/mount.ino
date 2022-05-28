#include <SD.h>

void setup()
{
    Serial.begin(115200);
    Serial.print("SD ");

    if (!SD.begin(SS))
    {
        Serial.println("failed to mount!");
        return;
    }
    Serial.println("mount!");
}

void loop()
{
}

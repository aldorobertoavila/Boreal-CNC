#include <SD.h>

const String EXT[] = { "ino" };

const uint8_t EXT_SIZE = 1;
const uint8_t FILES_SIZE = 2;

String paths[FILES_SIZE];

bool isFileExtension(String filename)
{
  String ext = filename.substring(filename.indexOf("."));

  ext.toLowerCase();

  for(uint8_t i = 0; i < EXT_SIZE; i++)
  {
    if(ext.indexOf(EXT[i]) > 0) return true;
  }

  return false;
}

uint8_t getFiles(File dir, String paths[], uint8_t length) 
{
  uint8_t i = 0;
  
  while (i < length) {
    File entry =  dir.openNextFile();

    if(!entry) break;

    String path = entry.name();

    if(path.indexOf("~") > 0) continue;

    if(isFileExtension(path))
    {
      paths[i] = path;
      /*
      if(entry.isDirectory()) 
      {
        paths[i] = filename + "/";
      }
      else
      {
        paths[i] = filename;
      }
      */

      i++;
    }

    entry.close();
  }

  return i;
}

void setup() {
  Serial.begin(115200);  
  
  while (!Serial);

  Serial.print("SD card initialization ");

  if (!SD.begin(SS)) {
    Serial.println("failed!");
    while (true);
  }

  Serial.println("finished!");

  File root = SD.open("/");
  int length = getFiles(root, paths, FILES_SIZE);
  
  for(uint8_t i = 0; i < length; i++) 
  {
    Serial.println(paths[i]);
  }
}

void loop() {

}
#include <SD.h>

#define MAX_FILES_SIZE 20

char INTERNAL_FILE_EXT = '~';
String GCODE_FILE_EXT = ".txt";
String SYSTEM_VOLUME = "System Volume Information";

String filePaths[MAX_FILES_SIZE];

bool isGcodeFile(String filename)
{
  String ext = filename.substring(filename.indexOf("."));
  ext.toLowerCase();
  return ext == GCODE_FILE_EXT;
}

bool isHiddenFile(String filename)
{
  return filename.indexOf(INTERNAL_FILE_EXT) > 0 || filename == SYSTEM_VOLUME;
}

uint8_t getFiles(File &root, String paths[], uint8_t maxFilePaths)
{
  File file;
  uint8_t i;
  String path;

  while (i < maxFilePaths)
  {
    file = root.openNextFile();

    if(!file) break;

    path = file.name();
    
    if(isHiddenFile(path))
    {
      file.close();
      continue;
    }

    if (file.isDirectory())
    {
      paths[i] = path + "/";
      i++;
    }
    else
    {
      if(isGcodeFile(path))
      {
        paths[i] = path;
        i++;
      }
    }

    file.close();
  }

  return i;
}

void setup()
{
  Serial.begin(115200);

  while (!Serial);

  Serial.print("SD ");

  if (!SD.begin(SS))
  {
    Serial.println("failed to mount!");
    return;
  }
  Serial.println("mount!");

  File root = SD.open("/");
  uint8_t n = getFiles(root, filePaths, MAX_FILES_SIZE);

  File file;
  String path;

  for(uint8_t i = 0; i < n; i++)
  {
    path = filePaths[i];
    
    Serial.println(path);

    file = SD.open("/" + path);
    
    if(file && !file.isDirectory())
    {
      while(file.available()){
          unsigned long start = micros();
          Serial.println(file.readStringUntil('\n'));
          Serial.println(micros() - start);
      }
    }
  
    file.close();
  }
}

void loop()
{
  delay(1);
}
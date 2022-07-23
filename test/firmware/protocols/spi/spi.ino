#include <Arduino.h>
#include <SPI.h>

SPIClass spi(VSPI);

void setup() {
  spi.begin();
}

void loop() {
  spi.beginTransaction(SPISettings(25000000, MSBFIRST, SPI_MODE0));
  digitalWrite(spi.pinSS(), LOW);
  spi.transfer(0xFF);
  digitalWrite(spi.pinSS(), HIGH);
  spi.endTransaction();
  
  delay(100);
}
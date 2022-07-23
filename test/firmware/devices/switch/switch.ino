#include <LimitSwitch.h>

#define SW_X_PIN 36
#define SW_Y_PIN 39
#define SW_Z_PIN 34

LimitSwitch SW_X(SW_X_PIN);
LimitSwitch SW_Y(SW_Y_PIN);
LimitSwitch SW_Z(SW_Z_PIN);

void setup() {
  Serial.begin(115200);

  pinMode(SW_X_PIN, INPUT);
  pinMode(SW_Y_PIN, INPUT);
  pinMode(SW_Z_PIN, INPUT);
}

void loop() {
  SW_X.tick();
  SW_Y.tick();
  SW_Z.tick();

  if(SW_X.isPressed())
  {
    Serial.println("Limit Switch X is pressed");
  }

  if(SW_Y.isPressed())
  {
    Serial.println("Limit Switch Y is pressed");
  }

  if(SW_Z.isPressed())
  {
    Serial.println("Limit Switch Z is pressed");
  }

  if(SW_X.isReleased())
  {
    Serial.println("Limit Switch X is released");
  }

  if(SW_Y.isReleased())
  {
    Serial.println("Limit Switch Y is released");
  }

  if(SW_Z.isReleased())
  {
    Serial.println("Limit Switch Z is released");
  }

  delay(10);
}

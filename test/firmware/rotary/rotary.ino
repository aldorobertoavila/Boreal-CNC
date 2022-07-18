#include <Rotary.h>

#define EN_CLK_PIN 2
#define EN_DT_PIN 4
#define EN_SW_PIN 15

Rotary rotary(EN_CLK_PIN, EN_DT_PIN, EN_SW_PIN);

void clickCallback()
{
    Serial.println("Click!");
}

void rotateCallback(Rotation direction)
{
    Serial.print("Position: ");
    Serial.print(rotary.getPosition());
    Serial.print(", Rotation: ");
  
    if(direction == Rotation::CLOCKWISE)
    {
      Serial.println("Clockwise");
    }
    else
    {
      Serial.println("Counterclockwise");
    }
}

void setup()
{
  Serial.begin(115200);

  rotary.setBoundaries(0, 10);
  rotary.setClickDebounceTime(250);
  rotary.setRotationDebounceTime(5);

  rotary.setOnClicked(clickCallback);
  rotary.setOnRotation(rotateCallback);
}

void loop()
{
  rotary.tick();
}
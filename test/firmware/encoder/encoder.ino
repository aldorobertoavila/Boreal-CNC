#include <util/atomic.h>
#include <Rotory.h>

#define EN_DT_PIN 2
#define EN_SW_PIN 3
#define EN_CLK_PIN 4

const int EN_DEBOUNCE = 5;
const int EN_START_POS = 50;
const int EN_LI = 0;
const int EN_LS = 100;

int pos;
int prevPos;

Rotory rotory(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

void tick()
{
    rotory.tick();
}

void setup()
{
    Serial.begin(115200);

    pinMode(EN_CLK_PIN, INPUT);
    pinMode(EN_DT_PIN, INPUT);
    pinMode(EN_SW_PIN, INPUT_PULLUP);

    rotory.setDebounceTime(EN_DEBOUNCE);
    rotory.setLowerBound(EN_LI);
    rotory.setPosition(EN_START_POS);
    rotory.setUpperBound(EN_LS);

    attachInterrupt(digitalPinToInterrupt(EN_DT_PIN), tick, LOW);
}

void loop()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        pos = rotory.getPosition();
    }

    if (pos != prevPos)
    {
        Serial.println(pos);
        prevPos = pos;
    }
}

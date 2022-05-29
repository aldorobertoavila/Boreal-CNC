#include <util/atomic.h>

#define EN_DT_PIN 2
#define EN_SW_PIN 3
#define EN_CLK_PIN 4

// Debounce Time (ms)
const int EN_DEBOUNCE = 5;
// Limit Inferior
const int EN_LI = 0;
// Limit Superior
const int EN_LS = 100;

volatile int volatile_pos = 50;
int prevPos = 50;

void encoder()
{
    static unsigned long prevMillis = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - prevMillis > EN_DEBOUNCE)
        if (digitalRead(EN_CLK_PIN))
        {
            volatile_pos++;
        }
        else
        {
            volatile_pos--;
        }

    volatile_pos = min(EN_LS, max(EN_LI, volatile_pos));
    prevMillis = currentMillis;
}

void setup()
{
    Serial.begin(115200);

    pinMode(EN_CLK_PIN, INPUT);
    pinMode(EN_DT_PIN, INPUT);
    pinMode(EN_SW_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(EN_DT_PIN), encoder, LOW);
}

void loop()
{
    int pos;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        pos = volatile_pos;
    }

    if (pos != prevPos)
    {
        Serial.println(pos);
        prevPos = pos;
    }
}

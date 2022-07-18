class Unit
{
public:
    Unit(float millis);

    uint16_t toSteps(float u, uint8_t steps);

private:
    float _millis;
};
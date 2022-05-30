class Rotory
{
public:
    Rotory();

    Rotory(uint8_t dt, uint8_t clk, uint8_t sw);

    void tick();
    volatile long getPosition();
    void setDebounceTime(long debounceTime);
    void setLowerBound(int lowerBound);
    void setPosition(long position);
    void setUpperBound(int upperBound);

private:
    volatile long _volatilePos;
    long _debounceTime;
    uint8_t _dt;
    uint8_t _clk;
    uint8_t _sw;
    int _lowerBound = INT32_MIN;
    int _upperBound = INT32_MAX;
};
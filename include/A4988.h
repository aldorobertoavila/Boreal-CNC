#include <Arduino.h>

#define MIN_PULSE_WIDTH 1

enum Direction
{
    DIRECTION_CCW,
    DIRECTION_CW
};

enum Resolution
{
    FULL,
    HALF,
    QUARTER,
    EIGHTH,
    SIXTEENTH
};

class A4988
{
public:
    A4988(uint8_t enaPin, uint8_t dirPin, uint8_t stepPin, uint8_t ms1Pin, uint8_t ms2Pin, uint8_t ms3Pin);

    void computeSpeed();

    void disable();

    long distanceTo();

    void enable();

    float getAcceleration();

    long getCurrentPosition();

    float getMaxSpeed();

    float getSpeed();

    long getTargetPosition();

    void move(long relative);

    void moveTo(long absolute);

    bool run();

    bool runSpeed();

    void setAcceleration(float acceleration);

    void setCurrentPosition(long position);

    void setMaxSpeed(float maxSpeed);

    void setResolution(Resolution resolution);

    void setSpeed(float speed);

    void step();

private:
    uint8_t _enaPin;
    uint8_t _dirPin;
    uint8_t _stepPin;
    uint8_t _ms1Pin;
    uint8_t _ms2Pin;
    uint8_t _ms3Pin;
    Direction _direction;
    Resolution _resolution;
    unsigned long _accelStepInterval;
    unsigned long _initialAccelInterval;
    unsigned long _minStepInterval;
    unsigned long _stepInterval;
    long _nSteps;
    long _currentPos;
    long _targetPos;
    long _lastStepTime;
    float _maxSpeed;
    float _acceleration;
    float _speed;
};
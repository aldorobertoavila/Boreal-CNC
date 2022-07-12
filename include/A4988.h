#include <Arduino.h>
#include <SPI.h>

enum Direction
{
    COUNTERCLOCKWISE,
    CLOCKWISE
};

enum Resolution
{
    FULL,
    HALF,
    QUARTER,
    EIGHTH,
    SIXTEENTH
};

class MotorInterface
{
public:

    virtual void reset();

    virtual void setEnable(bool enable);

    virtual void setResolution(Resolution res);

    virtual void setSleep(bool sleep);

    virtual void step(Direction dir);
};

class PinOutMotorInterface : public MotorInterface
{
public:
    PinOutMotorInterface(uint8_t enaPin, uint8_t dirPin, uint8_t resetPin, uint8_t stepPin, uint8_t sleepPin, uint8_t ms1Pin, uint8_t ms2Pin, uint8_t ms3Pin);

    void reset() override;

    void setEnable(bool enable) override;

    void setResolution(Resolution res) override;

    void setSleep(bool sleep) override;

    void step(Direction dir) override;

private:
    uint8_t _enaPin;
    uint8_t _dirPin;
    uint8_t _resetPin;
    uint8_t _sleepPin;
    uint8_t _stepPin;
    uint8_t _ms1Pin;
    uint8_t _ms2Pin;
    uint8_t _ms3Pin;
};

class ShiftRegisterMotorInterface : public MotorInterface
{
public:
    ShiftRegisterMotorInterface(SPIClass &spi, uint8_t csPin, long spiClk = 25000000L);

    void reset() override;

    void setEnable(bool enable) override;

    void setResolution(Resolution res) override;

    void setSleep(bool sleep) override;

    void step(Direction dir) override;

    void updateShiftOut();

private:
    long _spiClk;
    uint8_t _csPin;
    uint8_t _dataIn;
    SPIClass &_spi;
};

class A4988
{
public:

    A4988(MotorInterface &motorInterface);

    void computeSpeed();

    long distanceTo();

    float getAcceleration();

    long getCurrentPosition();

    float getMaxSpeed();

    float getSpeed();

    long getTargetPosition();

    void move(long relative);

    void moveTo(long absolute);

    void reset();

    bool run();

    bool runSpeed();

    void setAcceleration(float acceleration);

    void setCurrentPosition(long position);

    void setEnable(bool enable);

    void setMaxSpeed(float maxSpeed);

    void setResolution(Resolution res);

    void setSleep(bool sleep);

    void setSpeed(float speed);

    void step();

private:
    bool _enable;
    bool _sleep;
    Direction _direction;
    Resolution _resolution;
    MotorInterface &_motorInterface;
    unsigned long _accelStepInterval;
    unsigned long _initialAccelInterval;
    unsigned long _minStepInterval;
    unsigned long _stepInterval;
    long _nSteps;
    long _currentPos;
    long _targetPos;
    long _prevStepTime;
    float _maxSpeed;
    float _acceleration;
    float _speed;
};
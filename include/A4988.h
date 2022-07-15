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

    virtual void setEnable(bool enable);

    virtual void setReset(bool reset);

    virtual void setResolution(Resolution res);

    virtual void setSleep(bool sleep);

    virtual void step(Direction dir);
};

class ShiftRegisterMotorInterface : public MotorInterface
{
public:
    ShiftRegisterMotorInterface(SPIClass &spi, uint8_t csPin, long spiClk = 20000000L);

    void setEnable(bool enable) override;

    void setReset(bool reset) override;

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

    void disable();

    void enable();

    long distanceTo();

    float getAcceleration();

    long getCurrentPosition();

    float getMaxSpeed();

    float getSpeed();

    bool isEnable();

    bool isSleeping();

    long getTargetPosition();

    void move(long relative);

    void moveTo(long absolute);

    void reset();

    bool run();

    bool runSpeed();

    void setAcceleration(float acceleration);

    void setCurrentPosition(long position);

    void setMaxSpeed(float maxSpeed);

    void setResolution(Resolution res);

    void setSpeed(float speed);

    void sleep();

    void wakeUp();

protected:

    void computeSpeed();

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
#pragma once

#include <Arduino.h>
#include <MotorInterface.h>

class StepperMotor
{
public:
    StepperMotor(MotorInterface &motorInterface);

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

    void pause(unsigned long ms);

    void reset();

    bool run();

    bool runSpeed();

    void setAcceleration(float acceleration);

    void setCurrentPosition(long position);

    void setMaxSpeed(float maxSpeed);

    void setResolution(Resolution res);

    void setSpeed(float speed);

    void sleep();

    void stop();

    void wakeUp();

protected:
    void computeSpeed();

    void step();

private:
    bool _enable;
    bool _sleep;
    Rotation _rotation;
    Resolution _resolution;
    MotorInterface &_motorInterface;
    unsigned long _accelStepInterval;
    unsigned long _initialAccelInterval;
    unsigned long _minStepInterval;
    unsigned long _pauseInterval;
    unsigned long _pauseStartTime;
    unsigned long _stepInterval;
    long _nSteps;
    long _currentPos;
    long _targetPos;
    long _prevStepTime;
    float _maxSpeed;
    float _acceleration;
    float _speed;
};
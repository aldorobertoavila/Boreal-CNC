#pragma once

#include <Cartesian.h>
#include <Laser.h>

enum AutohomeState
{
    PRESS,
    RELEASE,
    RETURN,
    OFFSET
};

class Command
{
public:
    virtual bool continues() = 0;

    virtual void execute() = 0;

    virtual void setup() = 0;

    virtual void stop() = 0;
};

class ArcMoveCommand : public Command
{
public:
    ArcMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float i, float j, float k, float feedRate, uint8_t power);

    bool continues() override;

    void execute() override;

    void setup() override;

    void stop() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    float _x;
    float _y;
    float _z;
    float _i;
    float _j;
    float _k;
    float _feedRate;
    uint8_t _power;
};

class AutohomeCommand : public Command
{
public:
    AutohomeCommand(Cartesian &cartesian, Laser &laser);

    bool continues() override;

    void execute() override;

    void setup() override;

    void stop() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    AutohomeState _currentState;
};

class CircleMoveCommand : public Command
{
public:
    CircleMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float r, float feedRate, uint8_t power);

    bool continues() override;

    void execute() override;

    void setup() override;

    void stop() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    float _x;
    float _y;
    float _z;
    float _r;
    float _feedRate;
    uint8_t _power;
};

class DwellCommand : public Command
{
public:
    DwellCommand(unsigned long remainTime);

    bool continues() override;

    void execute() override;

    void setup() override;

    void stop() override;

private:
    unsigned long _remainTime; // ms
    unsigned long _startTime;
};

class LinearMoveCommand : public Command
{
public:
    LinearMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float feedRate, uint8_t power);

    bool continues() override;

    void execute() override;

    void setup() override;

    void stop() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    float _x;
    float _y;
    float _z;
    float _feedRate;
    uint8_t _power;
};

class SetPositioningCommand : public Command
{
public:
    SetPositioningCommand(Cartesian &cartesian, Positioning pos);

    bool continues() override;

    void execute() override;

    void setup() override{};

    void stop() override{};

private:
    Cartesian &_cartesian;
    Positioning _positioning;
};

class SetLengthUnitCommand : public Command
{
public:
    SetLengthUnitCommand(Cartesian &cartesian, LengthUnit unit);

    bool continues() override;

    void execute() override;

    void setup() override{};

    void stop() override{};

private:
    Cartesian &_cartesian;
    LengthUnit _unit;
};

class LaserOnCommand : public Command
{
public:
    LaserOnCommand(Laser &laser, uint8_t power, InlineMode mode);

    bool continues() override;

    void execute() override;

    void setup() override{};

    void stop() override{};

private:
    Laser &_laser;
    InlineMode _mode;
    uint8_t _power;
};

class LaserOffCommand : public Command
{
public:
    LaserOffCommand(Laser &laser);

    bool continues() override;

    void execute() override;

    void setup() override{};

    void stop() override{};

private:
    Laser &_laser;
};

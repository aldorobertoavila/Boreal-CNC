#pragma once

#include <Cartesian.h>
#include <Laser.h>
#include <Status.h>

enum AutohomeState
{
    PRESS,
    RELEASE,
    RETURN
};

class Command
{
public:
    virtual void complete() = 0;

    virtual void execute() = 0;

    virtual Status status() { return _currentStatus; };

    virtual void setup() = 0;

    virtual void stop() = 0;

protected:
    Status _currentStatus;
};

class InstantCommand : public Command
{
public:
    void complete() override{};

    Status status() override { return Status::COMPLETED; };

    void setup() override{};

    void stop() override{};
};

class MoveCommand : public Command
{
public:
    MoveCommand(Cartesian &cartesian, Laser &laser, float feedRate, uint8_t power);

    void complete() override;

    void stop() override;

protected:
    Cartesian &_cartesian;
    Laser &_laser;
    float _feedRate;
    uint8_t _power;
};

class ArcMoveCommand : public MoveCommand
{
public:
    ArcMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float i, float j, float k, float feedRate, uint8_t power);

    void execute() override;

    void setup() override;

private:
    float _x;
    float _y;
    float _z;
    float _i;
    float _j;
    float _k;
};

class AutohomeCommand : public MoveCommand
{
public:
    AutohomeCommand(Cartesian &cartesian, Laser &laser);

    void execute() override;

    void setup() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    Axis _currentAxis;
    AutohomeState _currentState;
};

class CircleMoveCommand : public MoveCommand
{
public:
    CircleMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float r, float feedRate, uint8_t power);

    void execute() override;

    void setup() override;

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

    void complete() override;

    void execute() override;

    void setup() override;

    void stop() override;

private:
    unsigned long _remainTime; // ms
    unsigned long _startTime;
};

class LinearMoveCommand : public MoveCommand
{
public:
    LinearMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float feedRate, uint8_t power);

    void execute() override;

    void setup() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    Axis _currentAxis;
    float _x;
    float _y;
    float _z;
    float _feedRate;
    uint8_t _power;
};

class SetPositioningCommand : public InstantCommand
{
public:
    SetPositioningCommand(Cartesian &cartesian, Positioning pos);

    void execute() override;

private:
    Cartesian &_cartesian;
    Positioning _pos;
};

class SetUnitCommand : public InstantCommand
{
public:
    SetUnitCommand(Cartesian &cartesian, Unit unit);

    void execute() override;

private:
    Cartesian &_cartesian;
    Unit _unit;
};

class LaserOnCommand : public InstantCommand
{
public:
    LaserOnCommand(Laser &laser, uint8_t power, InlineMode mode);

    void execute() override;

private:
    InlineMode _mode;
    uint8_t _power;
    Laser &_laser;
};

class LaserOffCommand : public InstantCommand
{
public:
    LaserOffCommand(Laser &laser);

    void execute() override;

private:
    Laser &_laser;
};

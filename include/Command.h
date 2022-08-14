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
    virtual void execute() = 0;

    virtual void finish() = 0;

    virtual void start() = 0;

    virtual Status status() = 0;
};

class InstantCommand : public Command
{
public:
    void start() override{};

    void finish() override{};

    Status status() override { return COMPLETED; };
};

class CancelableCommand : public Command
{
public:
    Status status() override { return _currentStatus; };

protected:
    Status _currentStatus;
};

class ArcMoveCommand : public CancelableCommand
{
public:
    ArcMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float i, float j, float k, uint8_t s);

    void execute() override;

    void finish() override;

    void start() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    float _x;
    float _y;
    float _z;
    float _i;
    float _j;
    float _k;
    uint8_t _s;
};

class AutohomeCommand : public CancelableCommand
{
public:
    AutohomeCommand(Cartesian &cartesian, Laser &laser);

    void execute() override;

    void finish() override;

    void start() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    Axis _currentAxis;
    AutohomeState _currentState;
};

class CircleMoveCommand : public CancelableCommand
{
public:
    CircleMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, float r, uint8_t s);

    void execute() override;

    void finish() override;

    void start() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    float _x;
    float _y;
    float _z;
    float _r;
    uint8_t _s;
};

class DwellCommand : public CancelableCommand
{
public:
    DwellCommand(unsigned long remainTime);

    void execute() override;

    void finish() override;

    void start() override;

private:
    unsigned long _remainTime; // ms
    unsigned long _startTime;
};

class LinearMoveCommand : public CancelableCommand
{
public:
    LinearMoveCommand(Cartesian &cartesian, Laser &laser, float x, float y, float z, uint8_t s);

    void execute() override;

    void finish() override;

    void start() override;

private:
    Cartesian &_cartesian;
    Laser &_laser;
    Axis _currentAxis;
    float _x;
    float _y;
    float _z;
    uint8_t _s;
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

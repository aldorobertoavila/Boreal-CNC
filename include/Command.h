#pragma once

#include <Cartesian.h>

enum CommandStatus
{
    COMPLETED,
    CONTINUE,
    ERROR
};

enum AutohomeState
{
    PRESS,
    RELEASE,
    RETURN
};

enum InlineMode
{
    ON,
    OFF
};

class Command
{
public:
    virtual void execute() {};

    virtual void start() {};

    virtual CommandStatus status() { return CONTINUE; };

};

class InstantCommand : public Command
{
public:
    void start() override{};

    CommandStatus status() override { return COMPLETED; };
};

class DurableCommand : public Command
{
public:
    CommandStatus status() override { return _currentStatus; };

protected:
    CommandStatus _currentStatus;
};

class ArcMoveCommand : public Command
{
public:
    ArcMoveCommand(Cartesian &cartesian, float x, float y, float z, float i, float j, float k, float r);

    void execute() override;

    void start() override;

private:
    Cartesian &_cartesian;
    float _x;
    float _y;
    float _z;
    float _i;
    float _j;
    float _k;
    float _r;
};

// TODO turn off laser
class AutohomeCommand : public DurableCommand
{
public:
    AutohomeCommand(Cartesian &cartesian);

    void execute() override;

    void start() override;

private:
    Cartesian &_cartesian;
    Axis _currentAxis;
    AutohomeState _currentState;
};

class DwellCommand : public DurableCommand
{
public:
    DwellCommand(unsigned long remainTime);

    void execute() override;

    void start() override;

private:
    unsigned long _remainTime; // ms
    unsigned long _startTime;
};

// TODO turn off laser
class LinearMoveCommand : public DurableCommand
{
public:
    LinearMoveCommand(Cartesian &cartesian, float x, float y, float z);

    void execute() override;

    void start() override;

private:
    Cartesian &_cartesian;
    float _x;
    float _y;
    float _z;
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

/*
class LaserOnCommand : public Command
{
public:
    LaserOnCommand(uint8_t pwm, InlineMode mode);

    void execute() override;

    void start() override;

    CommandStatus status() override;

private:
    InlineMode _mode;
    uint8_t _pwm;
};

class LaserOffCommand : public Command
{
public:
    LaserOffCommand();

    void execute() override;

    void start() override;

    CommandStatus status() override;
};
*/

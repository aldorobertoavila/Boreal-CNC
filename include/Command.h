#include <Cartesian.h>

enum CommandStatus
{
    COMPLETED,
    CONTINUE,
    ERROR
};

class Command
{
public:
    Command();

    virtual void execute();

    virtual void start();

    virtual CommandStatus status();
};

class LinearMoveCommand : public Command
{
public:
    LinearMoveCommand(Cartesian &cartesian, float x, float y, float z);

    void execute() override;

    void start() override;

    CommandStatus status() override;

private:
    Cartesian &_cartesian;
    float _x;
    float _y;
    float _z;
};

    enum AutohomeState
    {
        PRESS,
        RELEASE,
        RETURN
    };


class AutohomeCommand : public Command
{
public:
    AutohomeCommand(Cartesian &cartesian);

    void execute() override;

    void start() override;

    CommandStatus status() override;

private:
    Cartesian &_cartesian;
    Axis _currentAxis;
    AutohomeState _currentState;
    CommandStatus _currentStatus;
};

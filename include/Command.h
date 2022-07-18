#include <Cartesian.h>

enum Status
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

    virtual Status status();

};

class LinearMoveCommand : public Command
{
public:
    LinearMoveCommand(Cartesian &cartesian, float x, float y, float z);

    void execute() override;

    void start() override;

    Status status() override;

private:
    Cartesian &_cartesian;
    float _x;
    float _y;
    float _z;
};

class AutohomeCommand : public Command
{
public:
    AutohomeCommand(Cartesian &cartesian);

    void execute() override;

    void start() override;

    Status status() override;

private:
    Cartesian &_cartesian;
};

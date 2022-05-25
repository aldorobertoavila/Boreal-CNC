#include <Cartesian.h>
#include <Command.h>

class MoveCommand : Command
{
public:
    MoveCommand(Cartesian *cartesian, float *coordinate)
    {
        _cartesian = cartesian;
        _coordinate = coordinate;
    }

protected:
    Cartesian *_cartesian;
    float *_coordinate;
};

class LinearMoveCommand : MoveCommand
{
public:
    LinearMoveCommand(Cartesian *cartesian, float *coordinate) : MoveCommand(cartesian, coordinate)
    {
    }

    void start() override
    {
        _cartesian->setAcceleration(0);
        _cartesian->setTarget(_coordinate);
    }

    void execute() override
    {
        _cartesian->travel();
    }

    bool completed() override
    {
        // TODO check if target reached
        return false;
    }
};
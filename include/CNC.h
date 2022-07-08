#include <Cartesian.h>

class CNC
{
public:
    CNC(Cartesian &cartesian);

    void update();

private:
    Cartesian &_cartesian;
};
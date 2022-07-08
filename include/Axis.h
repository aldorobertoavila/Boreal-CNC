#include <A4988.h>

class Axis
{
public:
    Axis(A4988 *drivers);

private:
    A4988 *_drivers;
};
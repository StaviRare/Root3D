#include "Calc.h"
#include "Transform.h"

Vector3 Transform::getForward() const
{
    return Vector3(
        Calc::Sin(eulerAngles.y) * Calc::Cos(eulerAngles.x),
        Calc::Sin(eulerAngles.x),
        -Calc::Cos(eulerAngles.y) * Calc::Cos(eulerAngles.x)
    ).normalized();
}

#include "Vector3.h"

Vector3& Vector3::operator+=(const Vector3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3& Vector3::operator-=(const Vector3& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

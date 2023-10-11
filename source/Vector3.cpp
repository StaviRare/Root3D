#include "Vector3.h"
#include "Math.h"

Vector3 Vector3::operator+(const Vector3& rhs) const
{
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3& Vector3::operator+=(const Vector3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3& Vector3::operator-=(const Vector3& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector3 Vector3::operator*(float scalar) const 
{
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3& Vector3::operator*=(float scalar) 
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}


float Vector3::magnitude() const
{
    return Math::sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::normalized() const
{
    float mag = magnitude();
    return Vector3(x / mag, y / mag, z / mag);
}

void Vector3::normalize()
{
    float mag = magnitude();
    x /= mag;
    y /= mag;
    z /= mag;
}
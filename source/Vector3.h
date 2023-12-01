#pragma once

#include <sstream>
#include "Math.h"

class Vector3
{
public:
    float x, y, z;

    Vector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z)
    {
    }

    float magnitude() const
    {
        return Math::Sqrt(x * x + y * y + z * z);
    }

    Vector3 normalized() const
    {
        float mag = magnitude();
        
        if (mag > 0)
        {
            return Vector3(x / mag, y / mag, z / mag);
        }

        return *this;
    }

    void normalize()
    {
        float mag = magnitude();

        if (mag > 0)
        {
            x /= mag;
            y /= mag;
            z /= mag;
        }
    }

    std::string toString() const
    {
        std::ostringstream oss;
        oss << "(" << x << ", " << y << ", " << z << ")";
        return oss.str();
    }

    Vector3 operator+(const Vector3& rhs) const
    {
        return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3& operator+=(const Vector3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;

        return *this;
    }

    Vector3 operator-(const Vector3& rhs) const
    {
        return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3& operator-=(const Vector3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;

        return *this;
    }

    Vector3 operator*(float scalar) const
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;

        return *this;
    }
};

#pragma once

#include <sstream>
#include <string>
#include "Calc.h"
#include "Types.h"
#include "Vector3.h"

class Quaternion
{
    public:
    float x, y, z, w;

    Quaternion(float _x = 0, float _y = 0, float _z = 0, float _w = 1)
        : x(_x), y(_y), z(_z), w(_w)
    {}

    static Quaternion Identity()
    {
        return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    }

    const Vector3 ToEuler() const
    {
        // Roll (x-axis rotation)
        float sinr_cosp = 2 * ( w * x + y * z );
        float cosr_cosp = 1 - 2 * ( x * x + y * y );
        float roll = std::atan2(sinr_cosp, cosr_cosp);

        // Pitch (y-axis rotation)
        float sinp = 2 * ( w * y - z * x );
        float pitch;

        if (std::abs(sinp) >= 1)
        {
            pitch = std::copysign(Calc::PI / 2, sinp); // use 90 degrees if out of range
        }
        else
        {
            pitch = std::asin(sinp);
        }

        // Yaw (z-axis rotation)
        float siny_cosp = 2 * ( w * z + x * y );
        float cosy_cosp = 1 - 2 * ( y * y + z * z );
        float yaw = Calc::ArcTan2(siny_cosp, cosy_cosp);

        return Vector3(roll, pitch, yaw);
    }

    float Magnitude() const
    {
        return Calc::Sqrt(w * w + x * x + y * y + z * z);
    }

    float SqrMagnitude() const
    {
        return w * w + x * x + y * y + z * z;
    }

    Quaternion Normalized() const
    {
        float mag = Magnitude();

        if (mag > 0)
        {
            return Quaternion(x / mag, y / mag, z / mag, w / mag);
        }

        return *this;
    }

    void Normalize()
    {
        float mag = Magnitude();

        if (mag > 0)
        {
            x /= mag;
            y /= mag;
            z /= mag;
            w /= mag;
        }
    }

    Quaternion Inverse() const
    {
        float magSqr = SqrMagnitude();

        if (magSqr > 0)
        {
            return Quaternion(-x / magSqr, -y / magSqr, -z / magSqr, w / magSqr);
        }

        return *this;
    }

    string ToString() const
    {
        std::ostringstream oss;
        oss << "(" << x << ", " << y << ", " << z << ", " << w << ")";
        return oss.str();
    }

    Quaternion& operator=(const Quaternion& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;

        return *this;
    }

    Quaternion operator+(const Quaternion& rhs) const
    {
        return Quaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    Quaternion& operator+=(const Quaternion& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;

        return *this;
    }

    Quaternion operator-(const Quaternion& rhs) const
    {
        return Quaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }

    Quaternion& operator-=(const Quaternion& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;

        return *this;
    }

    Quaternion operator*(const Quaternion& rhs) const
    {
        return Quaternion(
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
            w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z);
    }

    Quaternion& operator*=(const Quaternion& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    Quaternion operator*(float scalar) const
    {
        return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Quaternion& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;

        return *this;
    }

    Quaternion operator/(float scalar) const
    {
        return Quaternion(x / scalar, y / scalar, z / scalar, w / scalar);
    }

    Quaternion& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;

        return *this;
    }

    bool operator==(const Quaternion& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    bool operator!=(const Quaternion& rhs) const
    {
        return !( *this == rhs );
    }
};

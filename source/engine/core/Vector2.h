#pragma once

#include <sstream>
#include "Calc.h"
#include "Types.h"

class Vector2
{
    public:
    float x, y;

    Vector2(float _x = 0, float _y = 0) : x(_x), y(_y)
    {}

    float magnitude() const
    {
        return Calc::Sqrt(x * x + y * y);
    }

    float sqrMagnitude() const
    {
        return x * x + y * y;
    }

    Vector2 normalized() const
    {
        float mag = magnitude();
        if (mag > 0)
        {
            return Vector2(x / mag, y / mag);
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
        }
    }

    float cross(const Vector2& other) const
    {
        return x * other.y - y * other.x;
    }

    float dot(const Vector2& other) const
    {
        return x * other.x + y * other.y;
    }

    string toString() const
    {
        std::ostringstream oss;
        oss << "(" << x << ", " << y << ")";
        return oss.str();
    }

    Vector2& operator=(const Vector2& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        x = rhs.x;
        y = rhs.y;

        return *this;
    }

    Vector2 operator*(float scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;

        return *this;
    }

    Vector2 operator/(float scalar) const
    {
        return Vector2(x / scalar, y / scalar);
    }

    Vector2& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;

        return *this;
    }

    Vector2 operator-() const
    {
        return Vector2(-x, -y);
    }

    bool operator==(const Vector2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Vector2& rhs) const
    {
        return !( *this == rhs );
    }

    Vector2 operator+(const Vector2& rhs) const
    {
        return Vector2(x + rhs.x, y + rhs.y);
    }

    Vector2& operator+=(const Vector2& rhs)
    {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    Vector2 operator-(const Vector2& rhs) const
    {
        return Vector2(x - rhs.x, y - rhs.y);
    }

    Vector2& operator-=(const Vector2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    Vector2 operator*(const Vector2& other) const
    {
        return Vector2(x * other.x, y * other.y);
    }

    Vector2& operator*=(const Vector2& other)
    {
        x *= other.x;
        y *= other.y;

        return *this;
    }
};

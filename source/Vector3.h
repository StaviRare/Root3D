#pragma once

class Vector3
{
public:
    float x, y, z;
    float magnitude() const;
    Vector3 normalized() const;
    void normalize();

    // Constructors
    Vector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    Vector3 operator+(const Vector3& rhs) const;
    Vector3& operator+=(const Vector3& rhs);

    Vector3 operator-(const Vector3& rhs) const;
    Vector3& operator-=(const Vector3& rhs);

    Vector3 operator*(float scalar) const;
    Vector3& operator*=(float scalar);
};


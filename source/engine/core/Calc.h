#pragma once

#include <cmath>

class Calc
{
    public:
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float HALF_PI = 0.5f * PI;
    static constexpr float TWO_PI = 2.0f * PI;
    static constexpr float DEG_TO_RAD = PI / 180.0f;
    static constexpr float RAD_TO_DEG = 180.0f / PI;

    static float ArcCos(float value)
    {
        return std::acos(value);
    }

    static float ArcSin(float value)
    {
        return std::asin(value);
    }

    static float ArcTan(float value)
    {
        return std::atan(value);
    }

    static float ArcTan2(float y, float x)
    {
        return std::atan2(y, x);
    }

    static float Clamp(float value, float min, float max)
    {
        return Max(min, Min(max, value));
    }

    static float Cos(float angle)
    {
        return std::cos(angle);
    }

    static float DegToRad(float degrees)
    {
        return degrees * DEG_TO_RAD;
    }

    static float Exp(float exponent)
    {
        return std::exp(exponent);
    }

    static float InvSqrt(float value)
    {
        return 1.0f / std::sqrt(value);
    }

    static float Lerp(float start, float end, float t)
    {
        return start + t * (end - start);
    }

    static float Log(float value)
    {
        return std::log(value);
    }

    static float Max(float a, float b)
    {
        return a > b ? a : b;
    }

    static float Min(float a, float b)
    {
        return a < b ? a : b;
    }

    static float Pow(float base, float exponent)
    {
        return std::pow(base, exponent);
    }

    static float RadToDeg(float radians)
    {
        return radians * RAD_TO_DEG;
    }

    static float Sin(float angle)
    {
        return std::sin(angle);
    }

    static float Sqrt(float value)
    {
        return std::sqrt(value);
    }

    static float Tan(float angle)
    {
        return std::tan(angle);
    }
};

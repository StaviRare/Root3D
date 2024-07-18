#include "Random.h"

unsigned int Random::state = 1;

void Random::InitState(unsigned int seed)
{
    state = seed;
}

float Random::Value()
{
    state = 1664525 * state + 1013904223;
    return ( state & 0xFFFFFF ) / static_cast<float>( 0x1000000 );
}

int Random::Range(int min, int max)
{
    return min + static_cast<int>( Value() * ( max - min + 1 ) );
}

float Random::Range(float min, float max)
{
    return min + ( max - min ) * Value();
}

Quaternion Random::Rotation()
{
    float u1 = Value();
    float u2 = Value();
    float u3 = Value();
    float sq1 = Calc::Sqrt(1.0f - u1);
    float sq2 = Calc::Sqrt(u1);
    float theta1 = Calc::TWO_PI * u2;
    float theta2 = Calc::TWO_PI * u3;
    return Quaternion(sq1 * Calc::Sin(theta1), sq1 * Calc::Cos(theta1), sq2 * Calc::Sin(theta2), sq2 * Calc::Cos(theta2));
}

Vector3 Random::OnUnitSphere()
{
    float theta = Calc::TWO_PI * Value();
    float phi = Calc::ArcCos(2.0f * Value() - 1.0f);
    return Vector3(Calc::Sin(phi) * Calc::Cos(theta), Calc::Sin(phi) * Calc::Sin(theta), Calc::Cos(phi));
}

Vector3 Random::InsideUnitSphere()
{
    float x, y, z;
    do
    {
        x = Range(-1.0f, 1.0f);
        y = Range(-1.0f, 1.0f);
        z = Range(-1.0f, 1.0f);
    }
    while (x * x + y * y + z * z >= 1.0f);
    return Vector3(x, y, z);
}

Vector2 Random::InsideUnitCircle()
{
    float x, y;
    do
    {
        x = Range(-1.0f, 1.0f);
        y = Range(-1.0f, 1.0f);
    }
    while (x * x + y * y >= 1.0f);
    return Vector2(x, y);
}

Color Random::ColorHSV(float minHue, float maxHue)
{
    float h = Range(minHue, maxHue);
    float s = Range(0.0f, 1.0f);
    float v = Range(0.0f, 1.0f);
    return Color(h, s, v, 1.0f);
}

#pragma once

#include <algorithm>
#include <sstream>

class Color
{
public:
    float r, g, b, a;

    Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f) : r(red), g(green), b(blue), a(alpha)
    {
    }

    std::string toString() const
    {
        std::ostringstream oss;
        oss << "Color(" << r << ", " << g << ", " << b << ", " << a << ")";
        return oss.str();
    }

    void clamp()
    {
        r = std::max(0.0f, std::min(1.0f, r));
        g = std::max(0.0f, std::min(1.0f, g));
        b = std::max(0.0f, std::min(1.0f, b));
        a = std::max(0.0f, std::min(1.0f, a));
    }

    static Color Red()
    {
        return Color(1.0f, 0.0f, 0.0f);
    }
    static Color Green()
    {
        return Color(0.0f, 1.0f, 0.0f);
    }
    static Color Blue()
    {
        return Color(0.0f, 0.0f, 1.0f);
    }
    static Color White()
    {
        return Color(1.0f, 1.0f, 1.0f);
    }
    static Color Black()
    {
        return Color(0.0f, 0.0f, 0.0f);
    }
    static Color Transparent()
    {
        return Color(0.0f, 0.0f, 0.0f, 0.0f);
    }

    Color operator+(const Color& rhs) const
    {
        return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
    }

    Color& operator+=(const Color& rhs)
    {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
        a += rhs.a;
        return *this;
    }

    Color operator*(float scalar) const
    {
        return Color(r * scalar, g * scalar, b * scalar, a * scalar);
    }

    Color& operator*=(float scalar)
    {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        a *= scalar;
        return *this;
    }
};

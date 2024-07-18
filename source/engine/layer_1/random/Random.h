#pragma once

#include "Calc.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"

class Random
{
    public:
    static void InitState(unsigned int seed = 1);
    static float Value();
    static int Range(int min, int max);
    static float Range(float min, float max);
    static Quaternion Rotation();
    static Vector3 OnUnitSphere();
    static Vector3 InsideUnitSphere();
    static Vector2 InsideUnitCircle();
    static Color ColorHSV(float minHue = 0.0f, float maxHue = 1.0f);

    private:
    static unsigned int state;
};

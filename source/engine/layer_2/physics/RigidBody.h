#pragma once

#include "Component.h"
#include "Vector3.h"

class RigidBody : public Component
{
    public:
    bool IsStatic = false;
    float Mass = 1.0f;
    float Drag = 0.0f;
    Vector3 Velocity = Vector3(0.0f, 0.0f, 0.0f);
    float AngularDrag = 0.05f;
    Vector3 AngularVelocity = Vector3(0.0f, 0.0f, 0.0f);
};

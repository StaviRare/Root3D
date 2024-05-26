#pragma once

#include "Vector3.h"
#include "Quaternion.h"

class Transform
{
    public:
    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 eulerAngles = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);
    Quaternion Rotation = Quaternion::Identity();

    // ToDo - Need to fix gimbal lock
    Vector3 getForward() const
    {
        return Vector3(
            std::sin(eulerAngles.y) * std::cos(eulerAngles.x),
            std::sin(eulerAngles.x),
            -std::cos(eulerAngles.y) * std::cos(eulerAngles.x)
        ).normalized();
    }
};

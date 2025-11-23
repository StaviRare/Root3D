#pragma once

#include "Vector3.h"
#include "Quaternion.h"

// TODO: make members private, add set/get for automatic angle/rotation sync

class Transform
{
    public:
    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 eulerAngles = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);
    Quaternion rotation = Quaternion::Identity();

    public:
    Vector3 getForward() const;
};

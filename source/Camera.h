#pragma once

#include "Vector3.h"

class Camera
{
public:
    // Constructor
    Camera();

    float fov;
    Vector3 position;
    Vector3 rotation;

    static bool exists();
    static Camera& getInstance();

private:
    static Camera* instance;
};


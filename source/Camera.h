#pragma once

#include "Entity.h"
#include "Vector3.h"
#include "Color.h"

class Camera : public Entity
{
public:
    // Constructor
    Camera();

    float fov = 60;
    float farClipPlane = 100;
    float nearClipPlane = 0.1f;
    Color backgroundColor;

    static bool exists();
    static Camera& getInstance();

private:
    static Camera* instance;
};


#pragma once

#include "Entity.h"
#include "Vector3.h"
#include "Color.h"

class Camera : public Entity
{
public:
    // Constructor
    Camera();

    float fov;
    Color backgroundColor;

    static bool exists();
    static Camera& getInstance();

private:
    static Camera* instance;
};


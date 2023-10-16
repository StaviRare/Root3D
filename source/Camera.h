#pragma once

#include "Entity.h"
#include "Vector3.h"

class Camera : public Entity
{
public:
    // Constructor
    Camera();

    float fov;

    static bool exists();
    static Camera& getInstance();

private:
    static Camera* instance;
};


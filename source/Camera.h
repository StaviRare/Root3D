#pragma once

#include "Entity.h"
#include "Vector3.h"
#include "Color.h"

class Camera : public Entity
{
    public:
    Camera();
    ~Camera();

    float fov = 60;
    float farClipPlane = 1000;
    float nearClipPlane = 0.3f;
    Color backgroundColor = Color(0, 0, 0, 0);

    float GetAspect();
    void ResetAspect();
    void SetAspect(float aspect);

    static bool Exists();
    static Camera& GetInstance();

    private:
    float customAspect = 0;
    bool isUsingCustomAspect = false;
    static Camera* Instance;
};


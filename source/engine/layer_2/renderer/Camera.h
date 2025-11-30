#pragma once

#include "Component.h"
#include "Vector3.h"
#include "Color.h"

class Camera : public Component
{
    public:
    float fov = 60;
    float farClipPlane = 1000;
    float nearClipPlane = 0.3f;
    Color backgroundColor = Color(0, 0, 0, 0);

    public:
    float GetAspect();
    void ResetAspect();
    void SetAspect(float aspect);
    void OnCreate() override;
    void OnDestroy() override;

    public:
    static bool Exists();
    static Camera* GetInstance();

    private:
    float customAspect = 0;
    bool isUsingCustomAspect = false;

    private:
    static Camera* instance;
};


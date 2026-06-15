#pragma once

#include <vector>

#include "Component.h"
#include "Vector3.h"
#include "Color.h"

class Camera : public Component
{
    public:
    int depth = 0;
    float fov = 60;
    float farClipPlane = 1000;
    float nearClipPlane = 0.3f;
    Color backgroundColor = Color(0.15f, 0.00f, 0.15f, 1);

    public:
    static bool Exists();
    static Camera* GetMainCamera();

    public:
    float GetAspect();
    void ResetAspect();
    void SetAspect(float aspect);
    void OnCreate() override;
    void OnDestroy() override;

    private:
    static std::vector<Camera*> s_cameras;

    private:
    float m_customAspect = 0;
    bool m_isUsingCustomAspect = false;
};


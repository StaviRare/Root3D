#pragma once

#include "Component.h"

class CameraController : public Component
{
    public:
    void OnCreate() override;
    void Tick() override;
    void OnDestroy() override;

    private:
    void HandleCamMovement();

    public:
    float m_cameraMoveSpeed = 3.00f;
    float m_cameraLookSpeed = 2.00f;

    private:
    Camera* m_camera = nullptr;
};

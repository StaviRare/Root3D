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
};

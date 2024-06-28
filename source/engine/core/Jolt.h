#pragma once

#include "PhysicsAPI.h"

class Jolt : public PhysicsAPI
{
    public:
    void Initialize() override;
    void Simulate() override;
    void UnInitialize() override;
};

#pragma once

#include "PhysicsAPI.h"
#include "JoltHelpers.h"

class Jolt : public PhysicsAPI
{
    public:
    void Initialize() override;
    void Simulate() override;
    void UnInitialize() override;
    void CreateBody(PhysCommand command) override;
    PhysResponse GetBodyData(uniqueID id) override;
};

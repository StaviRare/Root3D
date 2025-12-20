#pragma once

#include "IPhysicsAPI.h"
#include "JoltHelpers.h"

class Jolt : public IPhysicsAPI
{
    public:
    void Initialize() override;
    void Simulate() override;
    void UnInitialize() override;
    void CreateBody(PhysCommand command) override;
    PhysResponse GetBodyData(uniqueID id) override;
};

#pragma once

#include "PhysicsTypes.h"

class IPhysicsAPI
{
    public:
    virtual void Initialize() = 0;
    virtual void Simulate() = 0;
    virtual void UnInitialize() = 0;
    virtual void CreateBody(PhysCommand command) = 0;
    virtual PhysResponse GetBodyData(uniqueID id) = 0;

    protected:
    bool m_initialized = false;
};


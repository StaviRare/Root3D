#pragma once

class PhysicsAPI
{
    public:
    virtual void Initialize() = 0;
    virtual void Simulate() = 0;
    virtual void UnInitialize() = 0;

    protected:
    bool initialized = false;
};


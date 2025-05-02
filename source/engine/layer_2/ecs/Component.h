#pragma once

#include "Transform.h"

class Entity; // circular dependency

class Component
{
    friend Entity;

    private:
    Entity* owner = nullptr;

    public:
    Entity& GetEntity();
    Transform& GetTransform();

    private:
    void SetOwner(Entity& e);

    virtual void OnCreate() {}
    virtual void Tick() {}
    virtual void OnDestroy() {}
};
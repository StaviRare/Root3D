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
    virtual void LateTick() {}
    virtual void OnAppFocus(bool hasFocus) {}
    virtual void OnAppPause(bool isPaused) {}
    virtual void OnDestroy() {}
};
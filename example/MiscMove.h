#pragma once

#include "Component.h"

class MiscMove : public Component
{
    public:
    void Tick() override;

    public:
    Vector3 direction = Vector3(0, 0, 0);
};

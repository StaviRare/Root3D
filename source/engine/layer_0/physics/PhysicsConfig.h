#pragma once

#include "PhysicsAPI.h"
#include "Vector3.h"

struct PhysicsConfig
{
    Vector3 Gravity = Vector3(0, -9.81f, 0);
    PhysicsAPI PhysicsTypeAPI = PhysicsAPI::Null;
};

#pragma once

#include "PhysicsAPI.h"
#include "Vector3.h"

struct PhysicsDesc
{
    Vector3 gravity = Vector3(0, -9.81f, 0);
    PhysicsAPI physicsAPI = PhysicsAPI::Null;
};
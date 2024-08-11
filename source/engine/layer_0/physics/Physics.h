#pragma once

#include "Types.h"
#include "Vector3.h"
#include "PhysicsAPI.h"

enum class PhysicsType
{
    Null,
    Jolt
};

class Physics
{
    friend class Core;

    public:
    static string TypeName();
    static Vector3 GetGravity();
    static void SetGravity(Vector3 gravity); // float3
    static void CreateBody(PhysCommand command);
    static PhysResponse GetBodyData(uniqueID id);

    private:
    static Vector3 _gravity;
    static PhysicsAPI* _currentAPI;
    static PhysicsType _currentType;

    private:
    static void Initialize();
    static void Simulate();
    static void UnInitialize();
};
#pragma once

#include "Types.h"
#include "Vector3.h"
#include "IPhysicsAPI.h"
#include "PhysicsAPI.h"
#include "PhysicsConfig.h"

class Physics
{
    friend class Engine;

    public:
    static string TypeName();
    static Vector3 GetGravity();
    static void SetGravity(Vector3 gravity); // should be float3
    static void CreateBody(PhysCommand command);
    static PhysResponse GetBodyData(uniqueID id);

    private:
    static Vector3 _gravity;
    static IPhysicsAPI* _currentAPI;
    static PhysicsAPI _currentType;

    private:
    static void Initialize(PhysicsConfig config);
    static void Simulate();
    static void UnInitialize();
};
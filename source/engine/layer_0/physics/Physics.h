#pragma once

#include "Types.h"
#include "Vector3.h"
#include "PhysicsAPI.h"

enum class PhysicsType
{
    Null,
    Jolt
};

//struct PhysicsBodyData
//{
//    // body
//    int entityId;
//    bool updateBody;
//
//    // transform
//    float scale[3];
//    float position[3];
//    float rotation[4];
//
//    // rigidBody
//    float mass;
//    float drag;
//    bool isDynamic;
//
//    // collider
//    int colliderType;
//    float boxSize[3];
//    float sphereRadius;
//};
//
//struct PhysicsBodyUpdatedData
//{
//    float position[3];
//    float rotation[4];
//};

class Physics
{
    friend class Core;

    public:
    static string TypeName();
    static Vector3 GetGravity();
    static void SetGravity(Vector3 gravity);

    private:
    static Vector3 _gravity;
    static PhysicsAPI* _currentAPI;
    static PhysicsType _currentType;

    private:
    static void Initialize();
    static void Simulate();
    static void UnInitialize();
};
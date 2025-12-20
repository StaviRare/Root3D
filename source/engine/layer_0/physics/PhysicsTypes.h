#pragma once

struct PhysTransform
{
    float scale[3];
    float position[3];
    float rotation[4];
};

struct PhysRigidbody
{
    bool isStatic = false;
    float mass = 1.0f;
    float drag = 0.0f;
    float velocity[3];
    float angularDrag = 0.05f;
    float angularVelocity[3];
};

struct PhysCollider
{
    int type = 0; // 0 - None, 1 - Box, 2 - Sphere, 3 - Mesh
    const float* vertices;
    unsigned int verticesSize;
    float boxSize[3];
    float sphereRadius;
};

struct PhysCommand
{
    uniqueID entityID = -1;
    PhysTransform transform;
    PhysRigidbody rigidbody;
    PhysCollider collider;
};

struct PhysResponse
{
    bool hasData = false;
    float position[3];
    float rotation[4];
};

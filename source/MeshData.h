#pragma once
#include "Mesh.h"
#include "Component.h"

class MeshData : public Component
{
public:
    MeshData(); // Default constructor
    MeshData(const Mesh& _mesh); // Constructor taking a Mesh by reference

    Mesh mesh; // Use a Mesh object directly
};

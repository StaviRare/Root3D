#pragma once
#include "Mesh.h"
#include "Component.h"

class MeshData : public Component
{
public:
    MeshData(); 
    MeshData(const Mesh& _mesh);

    Mesh mesh;
};

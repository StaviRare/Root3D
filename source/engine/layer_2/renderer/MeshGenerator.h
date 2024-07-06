#pragma once

#include "Mesh.h"

class MeshGenerator {
public:
    static Mesh GetCube();
    static Mesh GetQuad();
    static Mesh GetSphere();
};
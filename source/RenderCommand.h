#pragma once

#include "Vector3.h"
#include "Texture.h"
#include "Mesh.h"

struct RenderCommand 
{
    Mesh* mesh;
    Texture* texture;
    Vector3 position;
    Vector3 eulerAngles;
    Vector3 scale;
};
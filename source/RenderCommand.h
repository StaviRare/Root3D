#pragma once

#include "Vector3.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"

struct RenderCommand 
{
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
    Vector3 position;
    Vector3 eulerAngles;
    Vector3 scale;
};
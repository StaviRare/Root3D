#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
public:
    Shader shader;
    Texture texture;

    Material() {}
    Material(const Shader& shader) : shader(shader) {}
};

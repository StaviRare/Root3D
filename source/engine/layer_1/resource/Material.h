#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
    public:
    Material(){} // REMOVE

    Material(const Shader& shader)
        : shader(shader)
    {}

    uniqueID GetShaderID()
    {
        return shader.resourceID;
    }

    public:
    Texture texture;

    private:
    Shader shader;
};

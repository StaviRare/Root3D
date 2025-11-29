#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
    public:
    Material() = default;

    Material(Shader& shader)
        : m_shader(&shader) {}

    uniqueID GetShaderID() const
    {
        return m_shader->getID();
    }

    public:
    Texture* texture;

    private:
    Shader* m_shader = nullptr;
};

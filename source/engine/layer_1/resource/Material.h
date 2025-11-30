#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
    public:
    Material() = default;

    Material(Shader& shader)
        : m_shader(&shader)
    {}

    uniqueID GetShaderID() const
    {
        uniqueID returnValue = 0;

        if (m_shader)
        {
            returnValue = m_shader->getResourceID();
        }

        return returnValue;
    }

    public:
    Texture* texture = nullptr;

    private:
    Shader* m_shader = nullptr;
};

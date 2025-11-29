#pragma once

#include "Types.h"
#include "Object.h"
#include "ShaderManager.h"
#include "Log.h"

class Shader : public Object
{
    public:
    Shader()
    {
        m_name = "Shader";
    }

    Shader(const string& vertexSource, const string& fragmentSource)
    {
        m_name = "Shader";
        m_resourceID = ShaderManager::CreateOrGet(vertexSource, fragmentSource);
    }

    ~Shader()
    {
        ShaderManager::TryDestroy(m_resourceID);
    }

    uniqueID getResourceID() const
    {
        return m_resourceID;
    }

    private:
    uniqueID m_resourceID = 0;
};
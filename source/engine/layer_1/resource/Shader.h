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
        resourceID = ShaderManager::CreateShader(vertexSource, fragmentSource);
    }

    //private:
    uniqueID resourceID = 0;
};


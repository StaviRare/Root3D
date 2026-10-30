#include "Shader.h"
#include "ShaderManager.h"

Shader::Shader(const string& name)
{
    m_name = "Shader";
    m_resourceID = ShaderManager::CreateOrGet(name);
}

Shader::~Shader()
{
    ShaderManager::TryDestroy(m_resourceID);
}

uniqueID Shader::getResourceID() const
{
    return m_resourceID;
}
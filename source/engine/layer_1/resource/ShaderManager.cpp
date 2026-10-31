#include <sstream>

#include "Log.h"
#include "ShaderManager.h"
#include "Graphics.h"
#include "AssetLoader.h"

uniqueID ShaderManager::m_nextID = 0;
std::unordered_map<uniqueID, Resource> ShaderManager::m_shaders;

uniqueID ShaderManager::GetHandle(uniqueID id)
{
    uniqueID returnValue = 0;

    auto it = m_shaders.find(id);

    if(it != m_shaders.end())
    {
        returnValue = it->second.handle;
    }

    return returnValue;
}

uniqueID ShaderManager::TryDestroy(uniqueID id)
{
    uniqueID returnValue = 0;
    auto it = m_shaders.find(id);

    if(it != m_shaders.end())
    {
        if(it->second.useCount > 1)
        {
            it->second.useCount--;
            returnValue = id;
        }
        else
        {
            Graphics::DestroyShader(it->second.handle);
            m_shaders.erase(it);
            ENGINE_DEBUG("Shader destroyed | ID: " + std::to_string(id));
        }
    }

    return returnValue;
}

uniqueID ShaderManager::CreateOrGet(const string& name)
{
    uniqueID returnValue = -1;
    string assetPath = BuildShaderAssetPath(name);
    bool assetPathValid = !assetPath.empty();

    if(assetPathValid){
        bool isNewShader = true;
        hash hash = AssetLoader::GetAssetHash(assetPath);

        // Check if shader with same hash already exists
        for (const auto& it : m_shaders)
        {
            const Resource& r = it.second;

            if (r.id == hash)
            {
                returnValue = it.first;
                m_shaders[returnValue].useCount++;
                isNewShader = false;
            }
        }

        if (isNewShader)
        {
            auto assetData = AssetLoader::LoadResource(assetPath);
            ShaderUpload upload = ParseShaderAsset(assetData);
            uniqueID shaderHandle = Graphics::CreateShader(upload);

            if (shaderHandle)
            {
                m_nextID++;
                Resource resource;
                resource.handle = shaderHandle;
                resource.useCount = 1;
                resource.id = hash;
                m_shaders[m_nextID] = resource;
                returnValue = m_nextID;
            }
        }

        bool shaderIsValid = returnValue > 0;

        if (shaderIsValid)
        {
            if (isNewShader)
            {
                ENGINE_DEBUG("Shader created | ID: " + std::to_string(returnValue));
            }
        }
    }

    return returnValue;
}

string ShaderManager::BuildShaderAssetPath(const string& name)
{
    string returnValue = "";
    string shaderFolder = "";
    string shaderExtension = "";
    string graphicsAPI = Graphics::TypeName();

    if (graphicsAPI == "OpenGL")
    {
        shaderFolder = "glsl";
        shaderExtension = ".glsl";
    }
    else if (graphicsAPI == "DirectX11")
    {
        shaderFolder = "hlsl";
        shaderExtension = ".hlsl";
    }
    else if (graphicsAPI == "OpenGLES1")
    {
        ENGINE_DEBUG("Failed to resolve shader path: OpenGLES1 does not support shaders.");
    }
    else if(graphicsAPI == "OpenGLES2")
    {
        shaderFolder = "gles2";
        shaderExtension = ".glsl";
    }
    else if(graphicsAPI == "Vulkan")
    {
        shaderFolder = "vulkan";
        shaderExtension = ".glsl";
    }
    else{
        ENGINE_ERROR("Failed to resolve shader path: unsupported graphics API " + graphicsAPI);
    }

    bool isSupportedAPI
        = !shaderFolder.empty() && !shaderExtension.empty();

    if(isSupportedAPI){
        returnValue = "shaders/" + shaderFolder + "/" + name + shaderExtension;
    }

    return returnValue;
}

ShaderUpload ShaderManager::ParseShaderAsset(const std::vector<char>& data)
{
    ShaderUpload returnValue;
    bool hasData = !data.empty();

    if (hasData)
    {
        string line;
        bool isFragment = false;
        string shaderString(data.begin(), data.end());
        std::istringstream fileStream(shaderString);
        std::stringstream vertexStream;
        std::stringstream fragmentStream;

        while (std::getline(fileStream, line))
        {
            if (line.find("==SEPARATOR==") != string::npos)
            {
                isFragment = true;
            }
            else
            {
                (isFragment ? fragmentStream : vertexStream) << line << '\n';
            }
        }

        returnValue.vertexCode = vertexStream.str();
        returnValue.fragmentCode = fragmentStream.str();
    }

    return returnValue;
}
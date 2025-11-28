#include "Log.h"
#include "ShaderManager.h"
#include "Graphics.h"
#include "ObjectManager.h"

// ToDo - if shader compile fail, should provide fallback

std::unordered_map<uniqueID, ShaderResource> ShaderManager::shaders;


uniqueID ShaderManager::CreateShader(const string vs, const string fs)
{
    //std::hash<std::string> hasher;
    //hash hashVS = hasher(vs);
    //hash hashFS = hasher(fs);
    //hash combinedHash = hashVS ^ (hashFS << 1);
    //ENGINE_ERROR("HASH: " + std::to_string(combinedHash));

    bool newShader = true;
    uniqueID returnValue = 0;

    for(const auto& it : shaders)
    {
        const ShaderResource& r = it.second;
        
        if(r.vertexCode == vs && r.fragmentCode == fs)
        {
            shaders[r.resourceID].useCount++;
            returnValue = r.resourceID;
            newShader = false;
        }
    }

    if (newShader)
    {
        ENGINE_ERROR("new");


        ShaderUpload upload;
        upload.vertexCode = vs;
        upload.fragmentCode = fs;
        unsigned int handleID = Graphics::CreateShader(upload);

        if (handleID)
        {
            uniqueID id = ObjectManager::GenerateId();

            ShaderResource res{};
            res.resourceID = handleID;
            res.useCount = 1;
            res.vertexCode = vs;
            res.fragmentCode = fs;
            res.handleID = handleID;

            shaders[id] = res;

            returnValue = handleID;
        }
        else
        {
            ENGINE_ERROR("Shader compile fail.");
        }
    }

    return returnValue;
}

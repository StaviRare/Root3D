#include "Resource.h"
#include <fstream>
#include <sstream>
#include <algorithm>

#include "stb_image.h"
#include "Debug.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture Resource::LoadTexture(const std::string& path)
{
    Texture texture;
    auto data = LoadResource(path);

    if (data.empty() == false)
    {
        int width, height, nrChannels;
        texture.rawData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data.data()), data.size(), &width, &height, &nrChannels, 0);

        if (texture.rawData)
        {
            texture.width = static_cast<unsigned int>(width);
            texture.height = static_cast<unsigned int>(height);
            texture.nrChannels = static_cast<unsigned int>(nrChannels);
        }
    }

    return texture;
}

Shader Resource::LoadShader(const std::string& path) {
    Shader shader;

    auto data = LoadResource(path);

    if (!data.empty()) {
        enum class ShaderType {
            NONE = -1,
            VERTEX = 0,
            FRAGMENT = 1
        };

        ShaderType type = ShaderType::NONE;

        std::string line;
        std::stringstream ss[2];
        std::string shaderString(data.begin(), data.end()); // Convert vector<char> to string
        std::istringstream fileStream(shaderString);

        while (std::getline(fileStream, line)) {
            if (line.find("--SHADER-END--") != std::string::npos) {
                type = ShaderType::FRAGMENT; // Switch to fragment shader
                continue; // Skip the separator line
            }

            if (type != ShaderType::NONE) {
                ss[static_cast<int>(type)] << line << '\n';
            }
            else {
                // Default to vertex shader if no type has been set yet
                type = ShaderType::VERTEX;
                ss[static_cast<int>(type)] << line << '\n';
            }
        }

        shader.vertexCode = ss[0].str();
        shader.fragmentCode = ss[1].str();
    }

    return shader;
}

std::vector<char> Resource::LoadResource(const std::string& resourcePath) {
    const std::string resourceFile = "C:/Users/Stavi/Desktop/3DEngineFoundation/build/resources.bin";

    std::ifstream file(resourceFile, std::ios::binary);

    if (!file)
    {
        Debug::error("Unable to open resource file: " + resourceFile);
        return {};
    }

    while (file)
    {
        unsigned int pathLength;
        file.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));

        if (!file)
        {
            break;
        }

        std::string path(pathLength, '\0');
        file.read(&path[0], pathLength);

        // Convert backslashes to forward slashes
        std::replace(path.begin(), path.end(), '\\', '/');

        unsigned int dataLength;
        file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));

        if (!file)
        {
            break;
        }

        if (path == resourcePath)
        {
            std::vector<char> data(dataLength);
            file.read(data.data(), dataLength);
            return data;
        }

        file.seekg(dataLength, std::ios::cur);
    }

    Debug::error("Resource not found: " + resourcePath);

    return {};
    
};
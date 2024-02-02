#include "Resource.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <windows.h>

#include "stb_image.h"
#include "Debug.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture Resource::LoadTexture(const string& path)
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

Shader Resource::LoadShader(const string& path) {
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

std::vector<char> Resource::LoadResource(const string& resourcePath)
{
    std::vector<char> data;
    const string resourceFile = GetResourcePath() + "/resources.bin";
    std::ifstream file(resourceFile, std::ios::binary);
    bool isFileOpen = file.is_open();
    bool isResourceFound = false;

    while (isFileOpen && !isResourceFound)
    {
        unsigned int pathLength;
        file.read(reinterpret_cast<char*>( &pathLength ), sizeof(pathLength));
        isFileOpen = file.good();

        if (isFileOpen)
        {
            string path(pathLength, '\0');
            file.read(&path[0], pathLength);
            std::replace(path.begin(), path.end(), '\\', '/');

            unsigned int dataLength;
            file.read(reinterpret_cast<char*>( &dataLength ), sizeof(dataLength));
            isFileOpen = file.good();

            if (isFileOpen && path == resourcePath)
            {
                data.resize(dataLength);
                file.read(data.data(), dataLength);
                isResourceFound = file.good();
            }
            else if (isFileOpen)
            {
                file.seekg(dataLength, std::ios::cur);
            }
        }
    }

    if (!isResourceFound && !resourceFile.empty())
    {
        Debug::LogError("Resource not found: " + resourcePath);
    }

    return data;
}


string WideCharToMultiByteString(const std::wstring& wide) {
    if (wide.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wide[0], (int)wide.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wide[0], (int)wide.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// ToDo - This is windows code. Put in platform code.
string Resource::GetResourcePath() 
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring widePath(buffer);
    std::wstring::size_type pos = widePath.find_last_of(L"\\/");
    std::wstring wideFolder = widePath.substr(0, pos);
    return WideCharToMultiByteString(wideFolder);
}
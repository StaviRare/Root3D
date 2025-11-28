#pragma once

#include "Types.h"

struct ShaderUpload
{
    const char* vertexCode = nullptr;
    const char* fragmentCode = nullptr;
};

struct TextureUpload
{
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int nrChannels = 0;
    const unsigned char* rawData = nullptr;
};

struct MeshUpload
{
    const float* vertices = nullptr;
    unsigned int verticesSize = 0;

    const float* texCoords = nullptr;
    unsigned int texCoordsSize = 0;

    const float* normals = nullptr;
    unsigned int normalsSize = 0;

    const int* indices = nullptr;
    unsigned int indicesSize = 0;
};

struct GPUHandle
{
    void* ptr = nullptr;

    explicit operator bool() const { return ptr != nullptr; }

    static GPUHandle Null()
    {
        return {};
    }
};


struct LightUniform
{
    enum Type { Directional = 0, Point = 1 } type;
    float color[4];
    float intensity;

    // Directional
    float direction[3];

    // Point
    float range;
    float position[3];
    float attenuation[3];
};

struct ObjectUniform
{
    GPUHandle mesh;
    GPUHandle shader;
    GPUHandle texture;
    float modelMatrix[16];
};

struct FrameUniform
{
    float backgroundColor[4];
    float viewMatrix[16];
    float projectionMatrix[16];
};
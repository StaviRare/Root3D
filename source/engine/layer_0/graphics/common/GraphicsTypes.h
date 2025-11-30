#pragma once

#include "Types.h"

struct ShaderUpload
{
    string vertexCode;
    string fragmentCode;
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

struct LightUniform
{
    unsigned int type; // 	0 - Directional, 1 - Point
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
    MeshUpload mesh;
    uniqueID shaderHandle;
    uniqueID textureHandle;
    float modelMatrix[16];
};

struct FrameUniform
{
    float backgroundColor[4];
    float viewMatrix[16];
    float projectionMatrix[16];

    LightUniform lights[20]; // for now. same in shader code.
};
#pragma once

#include "Texture.h"
#include "Shader.h"


struct LightRenderCommand
{
    int type; // 0 - directional, 1 - point
    float color[4];
    float intensity;

    // Directional
    float direction[3];

    // Point
    float range;
    float position[3];
    float attenuation[3];
};

struct ObjectRenderCommand
{
    Shader* shader;
    Texture* texture;

    const float* vertices;
    unsigned int verticesSize;

    const float* texCoords;
    unsigned int texCoordsSize;

    const float* normals;
    unsigned int normalsSize;

    const int* indices;
    unsigned int indicesSize;

    float modelMatrix[16];
};


struct GlobalRenderCommand
{
    float backgroundColor[4];
    float viewMatrix[16];
    float projectionMatrix[16];
};
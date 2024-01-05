#pragma once

#include "Texture.h"
#include "Shader.h"

struct ObjectRenderCommand // object render command
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
    float directionalLightDirection[3];
    float directionalLightColor[4];
    float viewMatrix[16];
    float projectionMatrix[16];
};
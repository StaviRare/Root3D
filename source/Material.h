#pragma once

#include "Texture.h"

class Material
{
public:
    Texture texture;
    unsigned char* vertexShader;
    unsigned char* fragmentShader;
};

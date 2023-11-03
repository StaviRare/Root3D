#pragma once

#include "Texture.h"

class Material
{
public:
    Texture texture;
    std::string vertexShader;
    std::string fragmentShader;
};

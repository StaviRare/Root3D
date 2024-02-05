#pragma once

#include <vector>
#include "Texture.h"
#include "Shader.h"
#include "Types.h"

class Resource
{
    public:
    static Shader LoadShader(const string& path);
    static Texture LoadTexture(const string& path);

    private:
    static std::vector<char> LoadResource(const string& resourcePath);
};

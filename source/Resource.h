#pragma once

#include <vector>
#include <string>
#include "Texture.h"
#include "Shader.h"

class Resource {
public:
    static Texture LoadTexture(const std::string& path);
    static Shader LoadShader(const std::string& path);

private:
    static std::vector<char> LoadResource(const std::string& resourcePath);
};

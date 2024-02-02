#pragma once

#include <vector>
#include <string>
#include "Texture.h"
#include "Shader.h"
#include "Types.h"


class Resource {
public:
    static Texture LoadTexture(const string& path);
    static Shader LoadShader(const string& path);

private:
    static std::vector<char> LoadResource(const string& resourcePath);
    static string GetResourcePath();
};

#pragma once

#include <unordered_map>

#include "Types.h"
#include "RenderCommand.h"

#include "Engine.h"

struct Res
{
    uniqueID resourceID;
    unsigned int useCount = 0;
};

struct ShaderResource : public Res
{
    uniqueID handleID;
    string vertexCode;
    string fragmentCode;
};









class ShaderManager
{
    public:
    static uniqueID CreateShader(const string vs, const string fs);

    private:
    static std::unordered_map<uniqueID, ShaderResource> shaders;
};
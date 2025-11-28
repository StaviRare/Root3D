#pragma once

#include "RenderCommand.h"

class GraphicsAPI
{
    public:
    virtual void Initialize() = 0;
    virtual void UnInitialize() = 0;
    virtual void BeginFrame(FrameUniform cmd) = 0;
    virtual void DrawObject(ObjectUniform cmd) = 0;
    virtual void EndFrame() = 0;

    virtual GPUHandle CreateTexture(const TextureUpload& data) = 0;
    virtual void DestroyTexture(GPUHandle handle) = 0;

    virtual GPUHandle CreateShader(const ShaderUpload& data) = 0;
    virtual void DestroyShader(GPUHandle handle) = 0;

    protected:
    bool initialized = false;
};


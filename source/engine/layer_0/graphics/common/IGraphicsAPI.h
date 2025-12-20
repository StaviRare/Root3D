#pragma once

#include "GraphicsTypes.h"

class IGraphicsAPI
{
    public:
    virtual void Initialize(void* windowHandle) = 0;
    virtual void UnInitialize() = 0;
    virtual void BeginFrame(FrameUniform cmd) = 0;
    virtual void DrawObject(ObjectUniform cmd) = 0;
    virtual void EndFrame() = 0;
    virtual void DestroyShader(uniqueID id) = 0;
    virtual void DestroyTexture(uniqueID id) = 0;
    virtual uniqueID CreateShader(const ShaderUpload data) = 0;
    virtual uniqueID CreateTexture(const TextureUpload data) = 0;

    protected:
    bool m_initialized = false;
};


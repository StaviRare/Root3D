#pragma once

#include "Types.h"
#include "IGraphicsAPI.h"

class Vulkan : public IGraphicsAPI
{
    public:
    void Initialize(void* windowHandle) override;
    void UnInitialize() override;
    void Resize(uint32_t width, uint32_t height) override;
    void OnSurfaceLost() override;
    void OnSurfaceRecreated(void* windowHandle) override;
    void BeginFrame(FrameUniform cmd) override;
    void DrawObject(ObjectUniform cmd) override;
    void EndFrame() override;
    void DestroyShader(uniqueID id) override;
    void DestroyTexture(uniqueID id) override;
    uniqueID CreateShader(const ShaderUpload data) override;
    uniqueID CreateTexture(const TextureUpload data) override;

    private:
    uniqueID m_nextTextureID = 0;
    uniqueID m_nextShaderID = 0;
    FrameUniform m_currentFrame;
};

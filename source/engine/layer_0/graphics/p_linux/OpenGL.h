#pragma once

#include <unordered_map>

#include "Types.h"
#include "IGraphicsAPI.h"

class OpenGL : public IGraphicsAPI
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
};

#pragma once
#include <unordered_map>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include "Types.h"
#include "IGraphicsAPI.h"

struct GLTexture
{
    GLuint textureID;
};

struct EGLHandles
{
    EGLDisplay display;
    EGLSurface surface;
};

class OpenGLES1 : public IGraphicsAPI
{
    public:
    void Initialize() override;
    void UnInitialize() override;
    void BeginFrame(FrameUniform cmd) override;
    void DrawObject(ObjectUniform cmd) override;
    void EndFrame() override;
    void DestroyShader(uniqueID id) override;
    void DestroyTexture(uniqueID id) override;
    uniqueID CreateShader(const ShaderUpload data) override;
    uniqueID CreateTexture(const TextureUpload data) override;

    private:
    static void OnWindowResize(int width, int height);

    private:
    bool m_initialized = false;
    uniqueID m_nextTextureID = 0;
    std::unordered_map<uniqueID, GLTexture> m_textureMap;
};

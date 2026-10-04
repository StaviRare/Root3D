#pragma once

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <unordered_map>

#include "Types.h"
#include "IGraphicsAPI.h"

struct GLTexture
{
    GLuint textureID;
};

struct GLShader
{
    GLuint programID;
};

struct EGLHandles
{
    EGLDisplay display;
    EGLSurface surface;
};

class OpenGLES2 : public IGraphicsAPI
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
    GLuint  CompileShader(const string& source, GLuint type);

    private:
    uniqueID m_nextTextureID = 0;
    uniqueID m_nextShaderID = 0;
    std::unordered_map<uniqueID, GLTexture> m_textureMap;
    std::unordered_map<uniqueID, GLShader> m_shaderMap;
    GLuint m_time = -1;
    GLuint m_indexBuffer = -1;
    GLuint m_vertexBuffer = -1;
    GLuint m_texCoordBuffer = -1;
    GLuint m_normalBuffer = -1;
    EGLHandles* m_eglHandles = nullptr;
    FrameUniform m_currentFrame;
};

#pragma once

#include <unordered_map>
#include <Windows.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "Types.h"
#include "IGraphicsAPI.h"

struct GLShader
{
    GLuint programID;
};

struct GLTexture
{
    GLuint textureID;
};

class OpenGL : public IGraphicsAPI
{
    public:
    void Initialize(void* windowHandle) override;
    void UnInitialize() override;
    void Resize(uint32_t width, uint32_t height) override;
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
    GLuint m_time = -1;
    GLuint m_indexBuffer = -1;
    GLuint m_vertexBuffer = -1;
    GLuint m_texCoordBuffer = -1;
    GLuint m_normalBuffer = -1;
    GLuint m_vertexArrayObject = -1;
    HDC m_deviceContext = nullptr;
    uniqueID m_nextShaderID = 0;
    uniqueID m_nextTextureID = 0;
    std::unordered_map<uniqueID, GLShader> m_shaderMap;
    std::unordered_map<uniqueID, GLTexture> m_textureMap;
    FrameUniform m_currentFrame;
};

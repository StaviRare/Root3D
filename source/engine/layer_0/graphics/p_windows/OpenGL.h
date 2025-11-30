#pragma once

#include <unordered_map>
#include <Windows.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "Types.h"
#include "IGraphicsAPI.h"
#include "Texture.h"

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
    GLuint  CompileShader(const string& source, GLuint type);

    private:
    bool m_initialized = false;
    GLuint m_indexBuffer;
    GLuint m_vertexBuffer;
    GLuint m_texCoordBuffer;
    GLuint m_normalBuffer;
    GLuint m_vertexArrayObject;
    HDC m_deviceContext;
    uniqueID m_nextShaderID = 0;
    uniqueID m_nextTextureID = 0;
    std::unordered_map<uniqueID, GLShader> m_shaderMap;
    std::unordered_map<uniqueID, GLTexture> m_textureMap;
    FrameUniform m_currentFrame;
};

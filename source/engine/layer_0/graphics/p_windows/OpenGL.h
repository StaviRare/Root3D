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
    unsigned int programID;
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
    void DestroyShader(uniqueID handle) override;
    void DestroyTexture(uniqueID handle) override;
    uniqueID CreateShader(const ShaderUpload data) override;
    uniqueID CreateTexture(const TextureUpload data) override;

    private:
    static void OnWindowResize(int width, int height);
    unsigned int CompileShader(const string& source, unsigned int type);

    private:
    // Shader uniform locations
    int modelLoc = -1;
    int viewLoc = -1;
    int projectionLoc = -1;
    int timeLoc = -1;
    int lightDirLoc = -1;
    int lightColorLoc = -1;
    int typeLoc = -1;
    int intensityLoc = -1;
    int rangeLoc = -1;
    int positionLoc = -1;
    int attenuationLoc = -1;
    int numLightsLoc = -1;

    FrameUniform _currentFrame;

    // Vertex and element buffers
    unsigned int VAO;
    unsigned int EBO;
    unsigned int VBO[3];

    HDC deviceContext;

    bool initialized = false;
    unsigned int nextShaderID = 0;
    unsigned int nextTextureID = 0;
    std::unordered_map<uniqueID, GLShader> m_shaderMap;
    std::unordered_map<uniqueID, GLTexture> m_textureMap;

    const int MAX_LIGHTS = 20;

};

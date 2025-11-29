#pragma once

#include "Types.h"
#include "IGraphicsAPI.h"

enum class GraphicsAPI
{
    Null,
    OpenGL,
    OpenGLES1,
    DirectX11
};

class Graphics
{
    friend class Engine;
    friend class RenderManager;
    friend class ShaderManager;
    friend class TextureManager;

    public:
    static string TypeName();

    private:
    static IGraphicsAPI* _currentAPI;
    static GraphicsAPI _currentType;

    private:
    static void Initialize();
    static void UnInitialize();
    static void BeginFrame(FrameUniform cmd);
    static void DrawObject(ObjectUniform cmd);
    static void EndFrame();

    static uniqueID CreateTexture(const TextureUpload data);
    static void DestroyTexture(uniqueID handle);
    static uniqueID CreateShader(const ShaderUpload data);
    static void DestroyShader(uniqueID handle);
};

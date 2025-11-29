#pragma once

#include "Types.h"
#include "GraphicsAPI.h"

enum class GraphicsType
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
    static GraphicsAPI* _currentAPI;
    static GraphicsType _currentType;

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

    protected:
    static bool initialized;
};

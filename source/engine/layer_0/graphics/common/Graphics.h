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
    friend class RenderCommandHandler;
    friend class ShaderManager;

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

    static GPUHandle CreateTexture(const TextureUpload& data);
    static void DestroyTexture(GPUHandle handle);

    static unsigned int CreateShader(const ShaderUpload data);
    static void DestroyShader(GPUHandle handle);

    protected:
    static bool initialized;
};

#pragma once

#include "Types.h"
#include "IGraphicsAPI.h"
#include "GraphicsConfig.h"
#include "GraphicsAPI.h"


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
    static void Initialize(GraphicsConfig config);
    static void UnInitialize();
    static void BeginFrame(FrameUniform cmd);
    static void DrawObject(ObjectUniform cmd);
    static void EndFrame();
    static void DestroyShader(uniqueID id);
    static void DestroyTexture(uniqueID id);
    static uniqueID CreateShader(const ShaderUpload data);
    static uniqueID CreateTexture(const TextureUpload data);
};

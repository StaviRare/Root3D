#pragma once

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <list>
#include "Types.h"
#include "GraphicsAPI.h"
#include "Texture.h"

struct EGLHandles
{
    EGLDisplay display;
    EGLSurface surface;
};

class OpenGLES1 : public GraphicsAPI
{
public:
    void Initialize() override;
    void ClearScreen() override;
    void ExecuteRenderCommands() override;
    void SwapFrameBuffers() override;
    void UnInitialize() override;

private:
    const int MAX_LIGHTS = 20;
    void BindTexture(Texture& texture);
    static void OnWindowResize(int width, int height);
};

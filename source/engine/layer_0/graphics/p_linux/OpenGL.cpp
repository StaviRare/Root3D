#include "Log.h"
#include "OpenGL.h"
#include "Timer.h"
#include "Calc.h"

void OpenGL::Initialize(void* windowHandle)
{
    // Imp later
}

void OpenGL::UnInitialize()
{
    // Imp later
}

void OpenGL::Resize(uint32_t width, uint32_t height)
{
    // Imp later
}

void OpenGL::OnSurfaceLost()
{
    // No need in windows
}

void OpenGL::OnSurfaceRecreated(void* windowHandle)
{
    // No need in windows
}

void OpenGL::BeginFrame(FrameUniform cmd)
{
    // Imp later
}

void OpenGL::DrawObject(ObjectUniform cmd)
{
    // Imp later
}

void OpenGL::EndFrame()
{
    // Imp later
}

void OpenGL::DestroyShader(uniqueID id)
{
    // Imp later
}

void OpenGL::DestroyTexture(uniqueID id)
{
    // Imp later
}

uniqueID OpenGL::CreateShader(const ShaderUpload data)
{
    // Imp later
    return 0;
}

uniqueID OpenGL::CreateTexture(const TextureUpload data)
{
    // Imp later
    return 0;
}
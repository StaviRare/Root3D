#include <GLES2/gl2.h>

#include "Log.h"
#include "Timer.h"
#include "Vulkan.h"

void Vulkan::Initialize(void* windowHandle)
{

}

void Vulkan::UnInitialize()
{

}

void Vulkan::Resize(uint32_t width, uint32_t height)
{

}

void Vulkan::OnSurfaceLost()
{
    // No need in linux
}

void Vulkan::OnSurfaceRecreated(void* windowHandle)
{
    // No need in linux
}

void Vulkan::BeginFrame(FrameUniform cmd)
{

}

void Vulkan::DrawObject(ObjectUniform cmd)
{

}

void Vulkan::EndFrame()
{

}

void Vulkan::DestroyShader(uniqueID id)
{

}

void Vulkan::DestroyTexture(uniqueID id)
{

}

uniqueID Vulkan::CreateShader(const ShaderUpload data)
{
    return 0;
}

uniqueID Vulkan::CreateTexture(const TextureUpload data)
{
    return 0;
}
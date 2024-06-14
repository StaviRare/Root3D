#include "Debug.h"
#include "OpenGLES1.h"
#include "JniBridge.h"
#include "Screen.h"
#include "RenderQueue.h"
#include "Timer.h"
#include "Calc.h"

void OpenGLES1::Initialize()
{
    Screen::RegisterResizeCallback(OnWindowResize);

    // display / surface
    EGLHandles* handles = static_cast<EGLHandles*>(Screen::GetNativeHandle());
    EGLDisplay display = handles->display;
    EGLSurface surface = handles->surface;

    if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE)
    {
        Debug::LogError("Invalid EGL handles");
        return;
    }

    // GL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
}

void OpenGLES1::ClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::vector<GLushort> ConvertIndices(const int* indices, unsigned int count) {
    std::vector<GLushort> convertedIndices(count);
    for (unsigned int i = 0; i < count; ++i) {
        convertedIndices[i] = static_cast<GLushort>(indices[i]);
    }
    return convertedIndices;
}

void OpenGLES1::ExecuteRenderCommands()
{
    const GlobalRenderCommand* onceCmd = RenderQueue::GetGlobalRenderCommand();

    if (onceCmd)
    {
        const float *bg = onceCmd->backgroundColor;
        const float *viewMatrix = onceCmd->viewMatrix;
        const float *projectionMatrix = onceCmd->projectionMatrix;

        glClearColor(bg[0], bg[1], bg[2], bg[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Projection matrix setup
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glLoadMatrixf(projectionMatrix);

        // Model view matrix setup
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(viewMatrix);

        auto lightCommands = RenderQueue::GetLightRenderCommands();
        auto objectCommands = RenderQueue::GetObjectRenderCommands();

        // Lighting. We're taking into account that every object works with lighting.
        glEnable(GL_LIGHTING);

        // Set global ambient light
        float globalAmbientColor[4] = {0.3f, 0.3f, 0.3f, 1.0f}; // ToDo! - Add this to Config 'GlobalAmbientColor'
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientColor);

        for (size_t i = 0; i < Calc::Min(lightCommands.size(), static_cast<size_t>(MAX_LIGHTS)); ++i)
        {
            const auto& light = lightCommands[i];
            GLenum lightID = GL_LIGHT0 + i;

            glEnable(lightID);
            glLightfv(lightID, GL_DIFFUSE, light.color);
            glLightfv(lightID, GL_SPECULAR, light.color);

            if (light.type == 0) // Directional light
            {
                float direction[4] = {light.direction[0], light.direction[1], light.direction[2], 0.0f};
                glLightfv(lightID, GL_POSITION, direction);
            }
            else if (light.type == 1) // Point light
            {
                float position[4] = {light.position[0], light.position[1], light.position[2], 1.0f};
                glLightfv(lightID, GL_POSITION, position);
                glLightf(lightID, GL_CONSTANT_ATTENUATION, light.attenuation[0]);
                glLightf(lightID, GL_LINEAR_ATTENUATION, light.attenuation[1]);
                glLightf(lightID, GL_QUADRATIC_ATTENUATION, light.attenuation[2]);
            }
        }

        // Objects drawing
        for (const auto& cmd : objectCommands)
        {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glLoadMatrixf(viewMatrix);
            glMultMatrixf(cmd.modelMatrix);

            // Vertex buffer
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, cmd.vertices);

            // Normal buffer
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, cmd.normals);

            if (cmd.texture)
            {
                Texture* texture = cmd.texture;
                BindTexture(*texture);

                // Enable 2D texture mapping and set texture coordinates
                glEnable(GL_TEXTURE_2D);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2, GL_FLOAT, 0, cmd.texCoords);
            }

            // Converting for compatibility with OpenGLES1
            std::vector<GLushort> convertedIndices = ConvertIndices(cmd.indices, cmd.indicesSize);
            glDrawElements(GL_TRIANGLES, cmd.indicesSize, GL_UNSIGNED_SHORT, convertedIndices.data());

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);

            // Disable for next objects
            if (cmd.texture)
            {
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                glDisable(GL_TEXTURE_2D);
            }
        }

        RenderQueue::Clear();
    }
}

void OpenGLES1::SwapFrameBuffers()
{
    EGLHandles* handles = static_cast<EGLHandles*>(Screen::GetNativeHandle());

    if (!eglSwapBuffers(handles->display, handles->surface))
    {
        Debug::LogError("Failed to swap buffers.");
    }
}

void OpenGLES1::UnInitialize()
{
    Screen::UnRegisterResizeCallback(OnWindowResize);
}

void OpenGLES1::BindTexture(Texture& texture)
{
    // Check if the texture is already loaded
    if (texture.textureID == 0)
    {
        // Generate texture ID and bind it
        glGenTextures(1, &texture.textureID);
        glBindTexture(GL_TEXTURE_2D, texture.textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        if (texture.rawData)
        {
            GLenum format = GL_RGBA;

            if (texture.nrChannels == 3)
            {
                format = GL_RGB;
            }

            glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.rawData);
            //glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            Debug::LogError("Failed to load texture");
        }
    }
    else
    {
        // Bind existing texture
        glBindTexture(GL_TEXTURE_2D, texture.textureID);
    }
}

void OpenGLES1::OnWindowResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

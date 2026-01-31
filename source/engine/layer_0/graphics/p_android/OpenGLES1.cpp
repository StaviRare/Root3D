#include <vector>

#include "Log.h"
#include "OpenGLES1.h"

void OpenGLES1::Initialize(void* windowHandle)
{
    handles = static_cast<EGLHandles*>(windowHandle);
    EGLDisplay display = handles->display;
    EGLSurface surface = handles->surface;

    bool validHandles = handles
        && handles->display != EGL_NO_DISPLAY
        && handles->surface != EGL_NO_SURFACE;

    if (validHandles)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glCullFace(GL_FRONT);

        // Need to move to GLES3. No light for now.
        // Light support. Hard coded for now. Should be in graphics desc.
//        glEnable(GL_LIGHTING);
//        float globalAmbient[4] = {0.3f, 0.3f, 0.3f, 1.0f};
//        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

        m_initialized = true;
    }
    else
    {
        ENGINE_ERROR("OpenGLES1::Initialize failed: invalid display or surface.");
    }
}

void OpenGLES1::UnInitialize()
{
    m_initialized = false;
}

void OpenGLES1::Resize(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
}

void OpenGLES1::OnSurfaceLost()
{
    m_initialized = false;
}

void OpenGLES1::OnSurfaceRecreated(void* windowHandle)
{
    handles = static_cast<EGLHandles*>(windowHandle);

    bool validHandles = handles
        && handles->display != EGL_NO_DISPLAY
        && handles->surface != EGL_NO_SURFACE;

    if (validHandles)
    {
        m_initialized = true;
    }
}

void OpenGLES1::BeginFrame(FrameUniform cmd)
{
    const float *bg = cmd.backgroundColor;
    const float *viewMatrix = cmd.viewMatrix;
    const float *projectionMatrix = cmd.projectionMatrix;

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

    auto lightCommands = cmd.lights;

    size_t numLights = sizeof(cmd.lights) / sizeof(cmd.lights[0]);

    for (size_t i = 0; i < numLights; ++i)
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
}

void OpenGLES1::DrawObject(ObjectUniform cmd)
{
    glPushMatrix();
    glMultMatrixf(cmd.modelMatrix);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, cmd.mesh.vertices);

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, cmd.mesh.normals);

    if (cmd.textureHandle != 0)
    {
        auto texIt = m_textureMap.find(cmd.textureHandle);
        if (texIt != m_textureMap.end())
        {
            glEnable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texIt->second.textureID);

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, cmd.mesh.texCoords);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
        }
    }

    std::vector<GLushort> convertedIndices(cmd.mesh.indicesSize);
    for (unsigned int i = 0; i < cmd.mesh.indicesSize; ++i)
        convertedIndices[i] = static_cast<GLushort>(cmd.mesh.indices[i]);

    glColor4f(1,1,1,1);
    glDrawElements(GL_TRIANGLES, cmd.mesh.indicesSize, GL_UNSIGNED_SHORT, convertedIndices.data());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();
}

void OpenGLES1::EndFrame()
{
    bool swapSucceeded
        = eglSwapBuffers(handles->display, handles->surface);

    if (swapSucceeded == false)
    {
        ENGINE_ERROR("EGL swap buffers failed.");
    }
}

void OpenGLES1::DestroyShader(uniqueID id)
{
    // No shaders in GLES1
}

void OpenGLES1::DestroyTexture(uniqueID id)
{
    auto it = m_textureMap.find(id);
    if (it != m_textureMap.end())
    {
        glDeleteTextures(1, &it->second.textureID);
        m_textureMap.erase(it);
    }
}

uniqueID OpenGLES1::CreateShader(const ShaderUpload data)
{
    // No shaders in GLES1
}

uniqueID OpenGLES1::CreateTexture(const TextureUpload data)
{
    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine format
    GLenum format = GL_RGBA;

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.rawData);

    // Store it
    m_nextTextureID++;
    GLTexture gpuTex{ texID };
    m_textureMap[m_nextTextureID] = gpuTex;

    return m_nextTextureID;
}
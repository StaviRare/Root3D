#include <GLES2/gl2.h>

#include "Log.h"
#include "Timer.h"
#include "OpenGLES2.h"

void OpenGLES2::Initialize(void* windowHandle)
{
    m_eglHandles = static_cast<EGLHandles*>(windowHandle);

    bool validHandles = m_eglHandles
        && m_eglHandles->display != EGL_NO_DISPLAY
        && m_eglHandles->surface != EGL_NO_SURFACE;

    if (validHandles)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glCullFace(GL_FRONT);
        glGenBuffers(1,&m_vertexBuffer);
        glGenBuffers(1,&m_texCoordBuffer);
        glGenBuffers(1,&m_normalBuffer);
        glGenBuffers(1,&m_indexBuffer);
        m_initialized = true;
    }
    else
    {
        ENGINE_ERROR("OpenGLES2::Initialize failed: invalid display or surface.");
    }
}

void OpenGLES2::UnInitialize()
{
    m_initialized = false;
}

void OpenGLES2::Resize(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
}

void OpenGLES2::OnSurfaceLost()
{
    // No need in linux
}

void OpenGLES2::OnSurfaceRecreated(void* windowHandle)
{
    // No need in linux
}

void OpenGLES2::BeginFrame(FrameUniform cmd)
{
    const float* bg = cmd.backgroundColor;
    glClearColor(bg[0], bg[1], bg[2], bg[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float* viewMatrix = cmd.viewMatrix;
    const float* projectionMatrix = cmd.projectionMatrix;
    m_currentFrame = cmd;
}

void OpenGLES2::DrawObject(ObjectUniform cmd)
{
    auto shaderIt = m_shaderMap.find(cmd.shaderHandle);

    if (shaderIt == m_shaderMap.end())
    {
        ENGINE_ERROR("Shader not found for DrawObject");
        return;
    }

    GLuint shaderProgram = shaderIt->second.programID;
    glUseProgram(shaderProgram);

    // Set model, view, projection matrices
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, cmd.modelMatrix);

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, m_currentFrame.viewMatrix);

    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, m_currentFrame.projectionMatrix);

    // Bind texture
    if (cmd.textureHandle != 0)
    {
        auto textureIt = m_textureMap.find(cmd.textureHandle);

        if (textureIt != m_textureMap.end())
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureIt->second.textureID);
            GLint textureLoc = glGetUniformLocation(shaderProgram, "tex0");
            glUniform1i(textureLoc, 0);
        }
    }

    // Time
    m_time = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(m_time, Timer::TimeSinceInit());

    // Lights
    size_t numLights =
        sizeof(m_currentFrame.lights) /
        sizeof(m_currentFrame.lights[0]);

    for (size_t i = 0; i < numLights; ++i)
    {
        const LightUniform& light = m_currentFrame.lights[i];
        std::string prefix = "lights[" + std::to_string(i) + "].";

        GLint typeLoc = glGetUniformLocation(shaderProgram, (prefix + "type").c_str());
        glUniform1i(typeLoc, light.type);

        GLint colorLoc = glGetUniformLocation(shaderProgram,(prefix + "color").c_str());
        glUniform3f(colorLoc, light.color[0], light.color[1], light.color[2]);

        GLint intensityLoc = glGetUniformLocation(shaderProgram, (prefix + "intensity").c_str());
        glUniform1f(intensityLoc, light.intensity);

        if (light.type == 0) // directional
        {
            GLint directionLoc = glGetUniformLocation(shaderProgram, (prefix + "direction").c_str());
            glUniform3f(directionLoc,light.direction[0],light.direction[1],light.direction[2]);
        }
        else if (light.type == 1)  // point
        {
            GLint positionLoc = glGetUniformLocation(shaderProgram, (prefix + "position").c_str());
            glUniform3f(positionLoc, light.position[0], light.position[1], light.position[2]);

            GLint rangeLoc = glGetUniformLocation(shaderProgram, (prefix + "range").c_str());
            glUniform1f(rangeLoc, light.range);

            GLint attenuationLoc = glGetUniformLocation(shaderProgram, (prefix + "attenuation").c_str());
            glUniform3f(attenuationLoc, light.attenuation[0], light.attenuation[1], light.attenuation[2]);
        }
    }

    GLint numLightsLoc = glGetUniformLocation(shaderProgram, "numLights");
    glUniform1i(numLightsLoc, static_cast<GLint>(numLights));

    // Vertex data
    glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,cmd.mesh.verticesSize * sizeof(float),cmd.mesh.vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),nullptr);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER,m_texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER,cmd.mesh.texCoordsSize * sizeof(float),cmd.mesh.texCoords,GL_STATIC_DRAW);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,2 * sizeof(float),nullptr);
    glEnableVertexAttribArray(1);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER,m_normalBuffer);
    glBufferData(GL_ARRAY_BUFFER,cmd.mesh.normalsSize * sizeof(float),cmd.mesh.normals,GL_STATIC_DRAW);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),nullptr);
    glEnableVertexAttribArray(2);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,cmd.mesh.indicesSize * sizeof(unsigned int),cmd.mesh.indices,GL_STATIC_DRAW);

    // Draw
    glDrawElements(GL_TRIANGLES,cmd.mesh.indicesSize,GL_UNSIGNED_INT,nullptr);

    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
        ENGINE_ERROR("OpenGLES2 DrawObject error: " +std::to_string(error));
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void OpenGLES2::EndFrame()
{
    bool swapSucceeded
        = eglSwapBuffers(m_eglHandles->display, m_eglHandles->surface);

    if (swapSucceeded == false)
    {
        ENGINE_ERROR("EGL swap buffers failed.");
    }
}

void OpenGLES2::DestroyShader(uniqueID id)
{
    auto it = m_shaderMap.find(id);
    if (it != m_shaderMap.end())
    {
        glDeleteProgram(it->second.programID);
        m_shaderMap.erase(it);
    }
}

void OpenGLES2::DestroyTexture(uniqueID id)
{
    auto it = m_textureMap.find(id);
    if (it != m_textureMap.end())
    {
        glDeleteTextures(1, &it->second.textureID);
        m_textureMap.erase(it);
    }
}

uniqueID OpenGLES2::CreateShader(const ShaderUpload data)
{
    GLuint vertexShader = CompileShader(data.vertexCode, GL_VERTEX_SHADER);

    if (!vertexShader)
    {
        ENGINE_ERROR("Failed to compile vertex shader");
        return 0;
    }

    GLuint fragmentShader = CompileShader(data.fragmentCode, GL_FRAGMENT_SHADER);

    if (!fragmentShader)
    {
        ENGINE_ERROR("Failed to compile fragment shader");
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glBindAttribLocation(program, 0, "aPos");
    glBindAttribLocation(program, 1, "aTexCoord");
    glBindAttribLocation(program, 2, "aNormal");
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program,GL_LINK_STATUS,&success);

    if (success == GL_FALSE)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        ENGINE_ERROR(infoLog);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);

        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_nextShaderID++;
    GLShader gpuShader{ program };
    m_shaderMap[m_nextShaderID] = gpuShader;

    return m_nextShaderID;
}

uniqueID OpenGLES2::CreateTexture(const TextureUpload data)
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.rawData);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

     // Store it
    m_nextTextureID++;
    GLTexture gpuTex{ texID };
    m_textureMap[m_nextTextureID] = gpuTex;
    return m_nextTextureID;
}

GLuint OpenGLES2::CompileShader(const string& source, GLuint type)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        ENGINE_ERROR(infoLog);
        glDeleteShader(shader);
        shader = 0;
    }

    return shader;
}
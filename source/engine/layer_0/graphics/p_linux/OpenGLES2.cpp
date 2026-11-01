#include <vector>

#include "Log.h"
#include "Timer.h"
#include "OpenGLES2.h"

void OpenGLES2::Initialize(void* windowHandle)
{
    WindowHandle* window
        = static_cast<WindowHandle*>(windowHandle);

    if(window == nullptr){
        ENGINE_ERROR("OpenGLES2::Initialize failed: invalid window handle.");
    }
    else{
        bool eglSuccess = InitializeEGL(window);
        bool glSuccess = InitializeGLES();

        m_initialized = eglSuccess && glSuccess;

        if(m_initialized == false){
            ENGINE_ERROR("OpenGLES2::Initialize failed: graphics initialization failed.");
        }
    }
}

void OpenGLES2::UnInitialize()
{
    DestroyOpenGL();
    DestroyEGL();
    m_initialized = false;
}

void OpenGLES2::Resize(uint32_t width, uint32_t height)
{
    if (m_eglWindow != nullptr)
    {
        wl_egl_window_resize(
            m_eglWindow,
            static_cast<int32_t>(width),
            static_cast<int32_t>(height),
            0,
            0);
    }

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
    m_currentFrame = cmd;
}

void OpenGLES2::DrawObject(ObjectUniform cmd)
{
    auto shaderIt = m_shaderMap.find(cmd.shaderHandle);
    auto meshIt = m_meshMap.find(cmd.meshHandle);

    if (shaderIt == m_shaderMap.end())
    {
        ENGINE_ERROR("Shader not found for DrawObject");
        return;
    }

    if (meshIt == m_meshMap.end())
    {
        ENGINE_ERROR("Mesh not found for DrawObject");
        return;
    }

    GLMesh& mesh = meshIt->second;
    GLuint shaderProgram = shaderIt->second.programID;

    glUseProgram(shaderProgram);

    // Matrices
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, cmd.modelMatrix);

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, m_currentFrame.viewMatrix);

    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, m_currentFrame.projectionMatrix);

    // Texture
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
    GLint timeLoc = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(timeLoc, Timer::TimeSinceInit());

    // Lights
    size_t numLights =
        sizeof(m_currentFrame.lights) /
        sizeof(m_currentFrame.lights[0]);

    for (size_t i = 0; i < numLights; i++)
    {
        const LightUniform& light = m_currentFrame.lights[i];
        std::string prefix = "lights[" + std::to_string(i) + "].";

        GLint typeLoc = glGetUniformLocation(shaderProgram, (prefix + "type").c_str());
        glUniform1i(typeLoc, light.type);

        GLint colorLoc = glGetUniformLocation(shaderProgram, (prefix + "color").c_str());
        glUniform3f(colorLoc, light.color[0], light.color[1], light.color[2]);

        GLint intensityLoc = glGetUniformLocation(shaderProgram, (prefix + "intensity").c_str());
        glUniform1f(intensityLoc, light.intensity);

        if (light.type == 0)
        {
            GLint directionLoc = glGetUniformLocation(shaderProgram, (prefix + "direction").c_str());
            glUniform3f(directionLoc, light.direction[0], light.direction[1], light.direction[2]);
        }
        else if (light.type == 1)
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

    // Vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    if (mesh.texCoordBuffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.texCoordBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
    }
    else
    {
        glDisableVertexAttribArray(1);
    }

    // Normals
    if (mesh.normalBuffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.normalBuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(2);
    }
    else
    {
        glDisableVertexAttribArray(2);
    }

    // Draw
    if (mesh.indexBuffer != 0 && mesh.indicesSize > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
        glDrawElements(GL_TRIANGLES, mesh.indicesSize, GL_UNSIGNED_SHORT, nullptr);
    }

    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
        ENGINE_ERROR("OpenGLES2 DrawObject error: " + std::to_string(error));
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLES2::EndFrame()
{
    bool swapSucceeded
        = eglSwapBuffers(m_eglDisplay, m_eglSurface);

    if (swapSucceeded == false)
    {
        ENGINE_ERROR("EGL swap buffers failed.");
    }
}

void OpenGLES2::DestroyMesh(uniqueID id)
{
    auto it = m_meshMap.find(id);

    if (it != m_meshMap.end())
    {
        GLMesh& mesh = it->second;
        glDeleteBuffers(1, &mesh.vertexBuffer);
        glDeleteBuffers(1, &mesh.texCoordBuffer);
        glDeleteBuffers(1, &mesh.normalBuffer);
        glDeleteBuffers(1, &mesh.indexBuffer);
        m_meshMap.erase(it);
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

uniqueID OpenGLES2::CreateMesh(const MeshUpload data)
{
    uniqueID returnValue = 0;

    bool valid = data.vertices != nullptr
                 && data.verticesSize > 0
                 && data.verticesSize % 3 == 0
                 && (data.texCoordsSize == 0 || data.texCoords != nullptr)
                 && (data.normalsSize == 0 || data.normals != nullptr)
                 && (data.indicesSize == 0 || data.indices != nullptr)
                 && data.indicesSize <= static_cast<unsigned int>(std::numeric_limits<GLsizei>::max());

    std::vector<GLushort> indices;

    if (valid)
    {
        unsigned int vertexCount = data.verticesSize / 3;

        for (unsigned int i = 0; i < data.indicesSize; i++)
        {
            if (data.indices[i] < 0
                || static_cast<unsigned int>(data.indices[i]) >= vertexCount
                || data.indices[i] > 65535)
            {
                valid = false;
                break;
            }

            indices.push_back(static_cast<GLushort>(data.indices[i]));
        }
    }

    if (valid)
    {
        GLMesh mesh;

        glGenBuffers(1, &mesh.vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            data.verticesSize * sizeof(float),
            data.vertices,
            GL_STATIC_DRAW);

        if (data.texCoordsSize > 0)
        {
            glGenBuffers(1, &mesh.texCoordBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.texCoordBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                data.texCoordsSize * sizeof(float),
                data.texCoords,
                GL_STATIC_DRAW);
        }

        if (data.normalsSize > 0)
        {
            glGenBuffers(1, &mesh.normalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.normalBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                data.normalsSize * sizeof(float),
                data.normals,
                GL_STATIC_DRAW);
        }

        if (!indices.empty())
        {
            glGenBuffers(1, &mesh.indexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(GLushort),
                indices.data(),
                GL_STATIC_DRAW);
        }

        GLenum error = glGetError();

        if (error == GL_NO_ERROR)
        {
            mesh.indicesSize = static_cast<GLsizei>(indices.size());

            m_nextMeshID++;
            m_meshMap[m_nextMeshID] = mesh;

            returnValue = m_nextMeshID;
        }
        else
        {
            ENGINE_ERROR("OpenGLES2::CreateMesh failed: " + std::to_string(error));

            glDeleteBuffers(1, &mesh.vertexBuffer);
            glDeleteBuffers(1, &mesh.texCoordBuffer);
            glDeleteBuffers(1, &mesh.normalBuffer);
            glDeleteBuffers(1, &mesh.indexBuffer);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    return returnValue;
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

void OpenGLES2::DestroyEGL()
{
    if (m_eglWindow != nullptr)
    {
        wl_egl_window_destroy(m_eglWindow);
        m_eglWindow = nullptr;
    }

    if (m_eglDisplay != EGL_NO_DISPLAY)
    {
        if (m_eglContext != EGL_NO_CONTEXT)
        {
            eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroyContext(m_eglDisplay, m_eglContext);
            m_eglContext = EGL_NO_CONTEXT;
        }

        if (m_eglSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(m_eglDisplay, m_eglSurface);
            m_eglSurface = EGL_NO_SURFACE;
        }

        eglTerminate(m_eglDisplay);
        m_eglDisplay = EGL_NO_DISPLAY;
    }
}

void OpenGLES2::DestroyOpenGL()
{
    if (m_vertexBuffer != 0) {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
    }

    if (m_texCoordBuffer != 0) {
        glDeleteBuffers(1, &m_texCoordBuffer);
        m_texCoordBuffer = 0;
    }

    if (m_normalBuffer != 0) {
        glDeleteBuffers(1, &m_normalBuffer);
        m_normalBuffer = 0;
    }

    if (m_indexBuffer != 0) {
        glDeleteBuffers(1, &m_indexBuffer);
        m_indexBuffer = 0;
    }
}

bool OpenGLES2::InitializeEGL(WindowHandle* window)
{
    bool success = true;

    m_eglWindow
        = wl_egl_window_create(window->surface, 800, 600);

    if (m_eglWindow == nullptr)
    {
        success = false;
    }

    if (success)
    {
        m_eglDisplay = eglGetDisplay(
            static_cast<EGLNativeDisplayType>(window->display));

        success = m_eglDisplay != EGL_NO_DISPLAY
                  && eglInitialize(m_eglDisplay, nullptr, nullptr)
                  && eglBindAPI(EGL_OPENGL_ES_API);
    }

    if (success)
    {
        const EGLint configAttributes[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };

        EGLConfig eglConfig = nullptr;
        EGLint configCount = 0;

        success = eglChooseConfig(
                      m_eglDisplay, configAttributes, &eglConfig, 1, &configCount)
                  && configCount > 0;

        if (success)
        {
            m_eglSurface = eglCreateWindowSurface(
                m_eglDisplay,
                eglConfig,
                reinterpret_cast<EGLNativeWindowType>(m_eglWindow),
                nullptr);

            success = m_eglSurface != EGL_NO_SURFACE;
        }

        if (success)
        {
            const EGLint contextAttributes[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE
            };

            m_eglContext = eglCreateContext(
                m_eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttributes);

            success = m_eglContext != EGL_NO_CONTEXT;
        }

        if (success)
        {
            success = eglMakeCurrent(
                          m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) == EGL_TRUE;
        }
    }

    return success;
}

bool OpenGLES2::InitializeGLES()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glGenBuffers(1, &m_vertexBuffer);
    glGenBuffers(1, &m_texCoordBuffer);
    glGenBuffers(1, &m_normalBuffer);
    glGenBuffers(1, &m_indexBuffer);

    return glGetError() == GL_NO_ERROR;
}
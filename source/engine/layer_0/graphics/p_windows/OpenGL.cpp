#include "Log.h"
#include "OpenGL.h"
#include "Timer.h"
#include "Calc.h"

void OpenGL::Initialize(void* windowHandle)
{
    HWND hwnd = reinterpret_cast<HWND>(const_cast<void*>(windowHandle));
    m_deviceContext = GetDC(hwnd);

    // Create OpenGL rendering context
    HGLRC hRC = wglCreateContext(m_deviceContext);
    wglMakeCurrent(m_deviceContext, hRC);

    m_initialized = glewInit() == GLEW_OK && m_deviceContext != nullptr;

    if (m_initialized)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glCullFace(GL_FRONT);

        // Create buffers
        glGenBuffers(1, &m_vertexBuffer);
        glGenBuffers(1, &m_texCoordBuffer);
        glGenBuffers(1, &m_normalBuffer);
        glGenBuffers(1, &m_indexBuffer);
        glGenVertexArrays(1, &m_vertexArrayObject);

        glBindVertexArray(m_vertexArrayObject);

        // Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        // Texture coordinates
        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(1);

        // Normals
        glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
}

void OpenGL::UnInitialize()
{
    HGLRC hRC = wglGetCurrentContext();

    // Release rendering context
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);

    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_texCoordBuffer);
    glDeleteBuffers(1, &m_normalBuffer);
    glDeleteBuffers(1, &m_indexBuffer);
    glDeleteVertexArrays(1, &m_vertexArrayObject);

    m_initialized = false;
}

void OpenGL::BeginFrame(FrameUniform cmd)
{
    const float* bg = cmd.backgroundColor;
    glClearColor(bg[0], bg[1], bg[2], bg[3]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float* viewMatrix = cmd.viewMatrix;
    const float* projectionMatrix = cmd.projectionMatrix;

    m_currentFrame = cmd;
}

void OpenGL::DrawObject(ObjectUniform cmd)
{
    // Get shader
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

    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, m_currentFrame.projectionMatrix);

    // Bind texture
    if (cmd.textureHandle != 0)
    {
        auto texIt = m_textureMap.find(cmd.textureHandle);
        if (texIt != m_textureMap.end())
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texIt->second.textureID);

            GLint texLoc = glGetUniformLocation(shaderProgram, "tex0");
            glUniform1i(texLoc, 0);
        }
    }

    // Set the 'time' uniform
    m_time = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(m_time, Timer::TimeSinceInit());

    size_t numLights = sizeof(m_currentFrame.lights) / sizeof(m_currentFrame.lights[0]);

    // Set lighting uniforms
    for (size_t i = 0; i < numLights; ++i)
    {
        const LightUniform& light = m_currentFrame.lights[i];
        std::string prefix = "lights[" + std::to_string(i) + "].";

        GLint typeLoc = glGetUniformLocation(shaderProgram, (prefix + "type").c_str());
        glUniform1i(typeLoc, light.type);

        GLint colorLoc = glGetUniformLocation(shaderProgram, (prefix + "color").c_str());
        glUniform3f(colorLoc, light.color[0], light.color[1], light.color[2]);

        GLint intensityLoc = glGetUniformLocation(shaderProgram, (prefix + "intensity").c_str());
        glUniform1f(intensityLoc, light.intensity);

        if (light.type == 0) // directional
        {
            GLint dirLoc = glGetUniformLocation(shaderProgram, (prefix + "direction").c_str());
            glUniform3f(dirLoc, light.direction[0], light.direction[1], light.direction[2]);
        }
        else if (light.type == 1) // point
        {
            GLint posLoc = glGetUniformLocation(shaderProgram, (prefix + "position").c_str());
            glUniform3f(posLoc, light.position[0], light.position[1], light.position[2]);

            GLint rangeLoc = glGetUniformLocation(shaderProgram, (prefix + "range").c_str());
            glUniform1f(rangeLoc, light.range);

            GLint attLoc = glGetUniformLocation(shaderProgram, (prefix + "attenuation").c_str());
            glUniform3f(attLoc, light.attenuation[0], light.attenuation[1], light.attenuation[2]);
        }
    }

    GLint numLightsLoc = glGetUniformLocation(shaderProgram, "numLights");
    glUniform1i(numLightsLoc, numLights);

    // Upload vertex data
    glBindVertexArray(m_vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, cmd.mesh.verticesSize * sizeof(float), cmd.mesh.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, cmd.mesh.texCoordsSize * sizeof(float), cmd.mesh.texCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, cmd.mesh.normalsSize * sizeof(float), cmd.mesh.normals, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cmd.mesh.indicesSize * sizeof(unsigned int), cmd.mesh.indices, GL_STATIC_DRAW);

    // Draw
    glDrawElements(GL_TRIANGLES, cmd.mesh.indicesSize, GL_UNSIGNED_INT, 0);

    // Unbind VAO for safety
    glBindVertexArray(0);
}

void OpenGL::EndFrame()
{
    SwapBuffers(m_deviceContext);
}

void OpenGL::DestroyShader(uniqueID id)
{
    auto it = m_shaderMap.find(id);
    if (it != m_shaderMap.end())
    {
        glDeleteProgram(it->second.programID);
        m_shaderMap.erase(it);
    }
}

void OpenGL::DestroyTexture(uniqueID id)
{
    auto it = m_textureMap.find(id);
    if (it != m_textureMap.end())
    {
        glDeleteTextures(1, &it->second.textureID);
        m_textureMap.erase(it);
    }
}

uniqueID OpenGL::CreateShader(const ShaderUpload data)
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
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
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

uniqueID OpenGL::CreateTexture(const TextureUpload data)
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

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Store it
    m_nextTextureID++;
    GLTexture gpuTex{ texID };
    m_textureMap[m_nextTextureID] = gpuTex;

    return m_nextTextureID;
}

void OpenGL::Resize(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
}

GLuint OpenGL::CompileShader(const string& source, GLuint  type)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}
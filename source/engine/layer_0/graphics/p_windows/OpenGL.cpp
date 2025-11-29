#include "Log.h"
#include "OpenGL.h"
#include "Timer.h"
#include "Screen.h"
#include "Calc.h"

void OpenGL::Initialize()
{
    Screen::RegisterResizeCallback(OnWindowResize);

    void* nativeHandle = Screen::GetNativeHandle();
    HWND hwnd = reinterpret_cast<HWND>(const_cast<void*>(nativeHandle));
    deviceContext = GetDC(hwnd);

    initialized = glewInit() == GLEW_OK && deviceContext != nullptr;

    if (initialized)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glCullFace(GL_FRONT);

        // Create vertex array and buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(3, VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        // Texture coordinates
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(1);

        // Normals
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(2);
    }
}

void OpenGL::UnInitialize()
{
    Screen::UnRegisterResizeCallback(OnWindowResize);

    glDeleteBuffers(3, VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void OpenGL::BeginFrame(FrameUniform onceCmd)
{
    const float* bg = onceCmd.backgroundColor;
    glClearColor(bg[0], bg[1], bg[2], bg[3]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float* viewMatrix = onceCmd.viewMatrix;
    const float* projectionMatrix = onceCmd.projectionMatrix;

    _currentFrame = onceCmd;
}

void OpenGL::DrawObject(ObjectUniform command)
{
    // Get shader
    auto shaderIt = m_shaderMap.find(command.shaderHandle);
    if (shaderIt == m_shaderMap.end())
    {
        ENGINE_ERROR("Shader not found for DrawObject");
        return;
    }
    GLuint shaderProgram = shaderIt->second.programID;
    glUseProgram(shaderProgram);

    // Set model, view, projection matrices
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, command.modelMatrix);

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, _currentFrame.viewMatrix);

    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, _currentFrame.projectionMatrix);

    // Bind texture
    if (command.textureHandle != 0)
    {
        auto texIt = m_textureMap.find(command.textureHandle);
        if (texIt != m_textureMap.end())
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texIt->second.textureID);

            GLint texLoc = glGetUniformLocation(shaderProgram, "tex0");
            glUniform1i(texLoc, 0);
        }
    }

    // Set lighting uniforms
    for (size_t i = 0; i < 20; ++i)
    {
        const LightUniform& light = _currentFrame.lights[i];
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
    glUniform1i(numLightsLoc, 20);

    // Upload vertex data
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, command.mesh.verticesSize * sizeof(float), command.mesh.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, command.mesh.texCoordsSize * sizeof(float), command.mesh.texCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, command.mesh.normalsSize * sizeof(float), command.mesh.normals, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, command.mesh.indicesSize * sizeof(unsigned int), command.mesh.indices, GL_STATIC_DRAW);

    // Draw
    glDrawElements(GL_TRIANGLES, command.mesh.indicesSize, GL_UNSIGNED_INT, 0);

    // Unbind VAO for safety
    glBindVertexArray(0);
}

void OpenGL::EndFrame()
{
    SwapBuffers(deviceContext);
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
    unsigned int vertexShader = CompileShader(data.vertexCode, GL_VERTEX_SHADER);
    if (!vertexShader)
    {
        ENGINE_ERROR("Failed to compile vertex shader");
        return 0;
    }

    unsigned int fragmentShader = CompileShader(data.fragmentCode, GL_FRAGMENT_SHADER);
    if (!fragmentShader)
    {
        ENGINE_ERROR("Failed to compile fragment shader");
        glDeleteShader(vertexShader);
        return 0;
    }

    unsigned int program = glCreateProgram();
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

    nextShaderID++;
    GLShader gpuShader{ program };
    m_shaderMap[nextShaderID] = gpuShader;

    return nextShaderID;
}

uniqueID OpenGL::CreateTexture(const TextureUpload data)
{
    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Set texture parameters (similar to D3D bind flags / filtering)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine format
    GLenum format = GL_RGBA;

    // Upload texture data (equivalent to D3D CreateTexture2D + initial data)
    glTexImage2D(GL_TEXTURE_2D, 0, format, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.rawData);

    // Generate mipmaps (optional; D3D only did 1 level in your example)
    glGenerateMipmap(GL_TEXTURE_2D);

    // Store in your map like D3D
    nextTextureID++;
    GLTexture gpuTex{ texID };
    m_textureMap[nextTextureID] = gpuTex;

    return nextTextureID;
}

void OpenGL::OnWindowResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

unsigned int OpenGL::CompileShader(const string& source, unsigned int type)
{
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    return shader;
}
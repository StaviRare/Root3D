#include <map>
#include <iostream>
#include <GL/glew.h>

#include "OpenGL.h"
#include "RenderQueue.h"
#include "Debug.h"
#include "Time.h"
#include "Screen.h"

// Shader uniform locations
static int modelLoc = -1;
static int viewLoc = -1;
static int projectionLoc = -1;
static int timeLoc = -1;
static int lightDirLoc = -1;
static int lightColorLoc = -1;
static int typeLoc = -1;
static int intensityLoc = -1;
static int rangeLoc = -1;
static int positionLoc = -1;
static int attenuationLoc = -1;
static int numLightsLoc = -1;

// Vertex and element buffers
static unsigned int VAO;
static unsigned int EBO;
static unsigned int VBO[3];

// Max supported lights
static const int MAX_LIGHTS = 100;

void OpenGL::Initialize()
{
    Screen::RegisterResizeCallback(OnWindowResize);

    initialized = glewInit() == GLEW_OK;

    if (initialized)
    {
        glEnable(GL_DEPTH_TEST);

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

void OpenGL::ClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL::ExecuteRenderCommands()
{
    if (initialized)
    {
        const GlobalRenderCommand* onceCmd = RenderQueue::GetGlobalRenderCommand();

        if (onceCmd)
        {
            const float* bg = onceCmd->backgroundColor;
            const float* viewMatrix = onceCmd->viewMatrix;
            const float* projectionMatrix = onceCmd->projectionMatrix;

            glClearColor(bg[0], bg[1], bg[2], bg[3]);

            auto lightCommands = RenderQueue::GetLightRenderCommands();
            auto objectCommands = RenderQueue::GetObjectRenderCommands();

            for (const auto& command : objectCommands)
            {
                // find / compile shader
                if (command.shader->ID == 0)
                {
                    command.shader->ID = CreateShaderProgram(command.shader->vertexCode, command.shader->fragmentCode);
                    shaderProgramIDs.push_back(command.shader->ID);
                }

                GLuint shaderProgram = command.shader->ID;
                glUseProgram(shaderProgram);

                // Retrieve the location of the 'time' uniform in the shader program
                timeLoc = glGetUniformLocation(shaderProgram, "time");

                // Set the 'time' uniform in the shader to the total elapsed time since program initialization
                glUniform1f(timeLoc, Time::TimeSinceInit());

                Texture* texture = command.texture;

                BindTexture(*texture);

                // Bind vertex position buffer and upload data
                glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
                glBufferData(GL_ARRAY_BUFFER, command.verticesSize * sizeof(float), command.vertices, GL_STATIC_DRAW);

                // Bind texture coordinate buffer and upload data
                glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
                glBufferData(GL_ARRAY_BUFFER, command.texCoordsSize * sizeof(float), command.texCoords, GL_STATIC_DRAW);

                // Bind vertex normal buffer and upload data
                glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
                glBufferData(GL_ARRAY_BUFFER, command.normalsSize * sizeof(float), command.normals, GL_STATIC_DRAW);

                // Bind index buffer and upload data
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, command.indicesSize * sizeof(int), command.indices, GL_STATIC_DRAW);

                // Set shader's model matrix
                modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, command.modelMatrix);

                // Set shader's view matrix
                viewLoc = glGetUniformLocation(shaderProgram, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMatrix);

                // Set shader's projection matrix
                projectionLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projectionMatrix);

                // Light. Respects the shader's maximum light limit (MAX_LIGHTS)
                for (size_t i = 0; i < std::min(lightCommands.size(), static_cast<size_t>(MAX_LIGHTS)); ++i)
                {
                    const auto& light = lightCommands[i];
                    string uniformPrefix = "lights[" + std::to_string(i) + "].";

                    typeLoc = glGetUniformLocation(shaderProgram, ( uniformPrefix + "type" ).c_str());
                    glUniform1i(typeLoc, light.type);

                    intensityLoc = glGetUniformLocation(shaderProgram, ( uniformPrefix + "intensity" ).c_str());
                    glUniform1f(intensityLoc, light.intensity);
                    
                    lightColorLoc = glGetUniformLocation(shaderProgram, ( uniformPrefix + "color" ).c_str());
                    glUniform3f(lightColorLoc, light.color[0], light.color[1], light.color[2]);

                    if (light.type == 0)  // Directional
                    {
                        lightDirLoc = glGetUniformLocation(shaderProgram, ( uniformPrefix + "direction" ).c_str());
                        glUniform3f(lightDirLoc, light.direction[0], light.direction[1], light.direction[2]);
                    }
                    else if (light.type == 1) // Point
                    {
                        rangeLoc = glGetUniformLocation(shaderProgram, ( uniformPrefix + "range" ).c_str());
                        glUniform1f(rangeLoc, light.range);

                        positionLoc = glGetUniformLocation(shaderProgram, ( uniformPrefix + "position" ).c_str());
                        glUniform3f(positionLoc, light.position[0], light.position[1], light.position[2]);
                        
                        attenuationLoc = glGetUniformLocation(shaderProgram, ( uniformPrefix + "attenuation" ).c_str());
                        glUniform3f(attenuationLoc, light.attenuation[0], light.attenuation[1], light.attenuation[2]);
                    }
                }

                // Don't forget to set the 'numLights' uniform to inform the shader how many lights there are
                numLightsLoc = glGetUniformLocation(shaderProgram, "numLights");
                glUniform1i(numLightsLoc, std::min(static_cast<int>(lightCommands.size()), MAX_LIGHTS));

                // Draw the mesh
                glBindVertexArray(VAO);

                // Draws the mesh as a series of triangles. 
                // The number of indices determines how many vertices are 
                // used from the index buffer, ensuring the entire mesh is rendered correctly.
                glDrawElements(GL_TRIANGLES, command.indicesSize, GL_UNSIGNED_INT, 0);
            }

            RenderQueue::Clear();
        }
    }
}

void OpenGL::UnInitialize()
{
    Screen::UnRegisterResizeCallback(OnWindowResize);

    glDeleteBuffers(3, VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    for (GLuint shaderID : shaderProgramIDs)
    {
        glDeleteProgram(shaderID);
    }

    shaderProgramIDs.clear();
}

void OpenGL::BindTexture(Texture& texture)
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
            glGenerateMipmap(GL_TEXTURE_2D);
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

unsigned int OpenGL::CreateShaderProgram(const string& vertexSource, const string& fragmentSource)
{
    unsigned int vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

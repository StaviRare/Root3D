#include <map>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OpenGL.h"
#include "RenderQueue.h"
#include "Debug.h"
#include "Time.h"

static GLFWwindow* window = nullptr;
static int modelLoc = -1;
static int viewLoc = -1;
static int projectionLoc = -1;
static int lightDirLoc = -1;
static int lightColorLoc = -1;

static unsigned int VAO;
static unsigned int EBO;
static unsigned int VBO[3];

static float screenWidth = 960;     // SHOULD NOT BE HERE
static float screenHeight = 540;    // SHOULD NOT BE HERE

OpenGL::OpenGL()
{

}

OpenGL::~OpenGL()
{

}

void OpenGL::Initialize()
{
    if (!glfwInit()) {
        //return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable window

    window = glfwCreateWindow(screenWidth, screenHeight, "Root3D", NULL, NULL);

    if (!window) {
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        glfwTerminate();
    }

    glEnable(GL_DEPTH_TEST);

    // Create vertex array and buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
}

void OpenGL::ClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL::ExecuteRenderCommands()
{
    if (!glfwWindowShouldClose(window))
    {
        const GlobalRenderCommand* onceCmd = RenderQueue::GetOnceCommand();
        
        if (onceCmd) 
        {
            const float* bg = onceCmd->backgroundColor;
            const float* viewMatrix = onceCmd->viewMatrix;
            const float* projectionMatrix = onceCmd->projectionMatrix;
            const float* lightColor = onceCmd->directionalLightColor;
            const float* lightDir = onceCmd->directionalLightDirection;

            glClearColor(bg[0], bg[1], bg[2], bg[3]);

            // Run render per object command
            while (RenderQueue::IsEmpty() == false)
            {
                ObjectRenderCommand* command = RenderQueue::Dequeue();

                // find / compile shader
                if (command->shader->ID == 0)
                {
                    command->shader->ID = CreateShaderProgram(command->shader->vertexCode, command->shader->fragmentCode);
                    shaderProgramIDs.push_back(command->shader->ID);
                }

                GLuint shaderProgram = command->shader->ID;
                glUseProgram(shaderProgram);

                // Retrieve the location of the 'time' uniform in the shader program
                GLint timeLocation = glGetUniformLocation(shaderProgram, "time");

                // Set the 'time' uniform in the shader to the total elapsed time since program initialization
                glUniform1f(timeLocation, Time::TimeSinceInit());

                Texture* texture = command->texture;

                BindTexture(*texture);

                // Bind vertex position buffer and upload data
                glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
                glBufferData(GL_ARRAY_BUFFER, command->verticesSize * sizeof(float), command->vertices, GL_STATIC_DRAW);

                // Bind texture coordinate buffer and upload data
                glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
                glBufferData(GL_ARRAY_BUFFER, command->texCoordsSize * sizeof(float), command->texCoords, GL_STATIC_DRAW);

                // Bind vertex normal buffer and upload data
                glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
                glBufferData(GL_ARRAY_BUFFER, command->normalsSize * sizeof(float), command->normals, GL_STATIC_DRAW);

                // Bind index buffer and upload data
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, command->indicesSize * sizeof(int), command->indices, GL_STATIC_DRAW);

                // Set shader's model matrix
                modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, command->modelMatrix);

                // Set shader's view matrix
                viewLoc = glGetUniformLocation(shaderProgram, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMatrix);

                // Set shader's projection matrix
                projectionLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projectionMatrix);

                // Set shader's light direction
                lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
                glUniform3f(lightDirLoc, lightDir[0], lightDir[1], lightDir[2]);

                // Set shader's light color
                lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
                glUniform3f(lightColorLoc, lightColor[0], lightColor[1], lightColor[2]);

                // Draw the mesh
                glBindVertexArray(VAO);

                // Draws the mesh as a series of triangles. 
                // The number of indices determines how many vertices are 
                // used from the index buffer, ensuring the entire mesh is rendered correctly.
                glDrawElements(GL_TRIANGLES, command->indicesSize, GL_UNSIGNED_INT, 0);
            }
        }
        // Swap buffers: This displays the rendered frame on the window.
        glfwSwapBuffers(window);

        // Poll events: This checks for user input and window events (e.g., keyboard, mouse).
        glfwPollEvents();
    }
    else
    {
        // Cleanup resources

        // Delete VBOs
        glDeleteBuffers(3, VBO);

        // Delete EBO
        glDeleteBuffers(1, &EBO);

        // Delete VAO
        glDeleteVertexArrays(1, &VAO);

        // Delete shader program
        for (GLuint shaderID : shaderProgramIDs) 
        {
            glDeleteProgram(shaderID);
        }

        shaderProgramIDs.clear();

        // Terminate GLFW
        glfwTerminate();
    }
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
            Debug::error("Failed to load texture");
        }
    }
    else
    {
        // Bind existing texture
        glBindTexture(GL_TEXTURE_2D, texture.textureID);
    }
}

unsigned int OpenGL::CompileShader(const string& source, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    return shader;
}

unsigned int OpenGL::CreateShaderProgram(const string& vertexSource, const string& fragmentSource) {
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

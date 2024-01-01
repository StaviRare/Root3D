#include <iostream>
#include <GL/glew.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#include "OpenGL.h"
#include "Mesh.h"
#include "Camera.h"
#include "RenderQueue.h"
#include "Debug.h"


static GLFWwindow* window = nullptr;
static int modelLoc = -1;
static int viewLoc = -1;
static int projectionLoc = -1;
static unsigned int VAO, VBO[3], EBO; // Separate VBOs for positions, texture coordinates, and normals
static float aspectRatio = 0;
static float screenWidth = 960;
static float screenHeight = 540;

OpenGL::OpenGL()
{

}

OpenGL::~OpenGL()
{

}

void onWindowResize(GLFWwindow* window, int width, int height)
{
    // Update the OpenGL viewport
    glViewport(0, 0, width, height);

    // protection
    if (height == 0) height = 1;

    screenWidth = width;
    screenHeight = height;
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}


void OpenGL::Initialize()
{
    if (!glfwInit()) {
        //return -1;
    }

    window = glfwCreateWindow(screenWidth, screenHeight, "Scene", NULL, NULL);
    aspectRatio = screenWidth / screenHeight;

    if (!window) {
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, onWindowResize); // on resize callback

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
        if (Camera::exists())
        {
            Camera& camera = Camera::getInstance();

            const auto& bg = camera.backgroundColor;
            glClearColor(bg.r, bg.g, bg.b, bg.a);


            while (!RenderQueue::IsEmpty()) {
                RenderCommand* command = RenderQueue::Dequeue();

                
                // find / compile shader
                if (command->shader->ID == 0) 
                {
                    command->shader->ID = CreateShaderProgram(command->shader->vertexCode, command->shader->fragmentCode);
                    shaderProgramIDs.push_back(command->shader->ID);
                }
                GLuint shaderProgram = command->shader->ID;
                glUseProgram(shaderProgram);


                // Set shader uniforms
                modelLoc = glGetUniformLocation(shaderProgram, "model");
                viewLoc = glGetUniformLocation(shaderProgram, "view");
                projectionLoc = glGetUniformLocation(shaderProgram, "projection");


                Mesh* mesh = command->mesh;
                Texture* texture = command->texture;
                Vector3 position = command->position;
                Vector3 rotation = command->eulerAngles;
                Vector3 scale = command->scale;

                BindTexture(*texture);


                // We're currently using one texture unity.
                //glActiveTexture(GL_TEXTURE0);



                // Update VBOs
                glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
                glBufferData(GL_ARRAY_BUFFER, mesh->GetVertices().size() * sizeof(float), mesh->GetVertices().data(), GL_STATIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
                glBufferData(GL_ARRAY_BUFFER, mesh->GetTexCoords().size() * sizeof(float), mesh->GetTexCoords().data(), GL_STATIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
                glBufferData(GL_ARRAY_BUFFER, mesh->GetNormals().size() * sizeof(float), mesh->GetNormals().data(), GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndices().size() * sizeof(int), mesh->GetIndices().data(), GL_STATIC_DRAW);

                glm::mat4 model = glm::mat4(1.0f);

                // Apply translation
                model = glm::translate(model, glm::vec3(position.x, position.y, position.z));

                // Apply scale
                model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

                // Only then, apply rotations
                model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the X-axis
                model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the Y-axis
                model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the Z-axis

                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                // Directly translate camera.position to glm::vec3
                glm::vec3 cameraPosGLM(camera.transform.position.x, camera.transform.position.y, camera.transform.position.z);

                // Convert the camera's forward direction, calculated from its Euler angles, to a glm::vec3 type.
                glm::vec3 forwardVector = glm::vec3(camera.transform.getForward().x, camera.transform.getForward().y, camera.transform.getForward().z);

                // Use the translated glm::vec3 for camera position
                glm::vec3 cameraTarget = cameraPosGLM + forwardVector;

                glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Upward direction

                // Use the translated glm::vec3 for the lookAt
                glm::mat4 view = glm::lookAt(cameraPosGLM, cameraTarget, cameraUp);
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

                // Set the projection matrix (perspective projection)
                glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspectRatio, camera.nearClipPlane, camera.farClipPlane);
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


                glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)); // Direction of the light
                glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // White light

                // Set light properties in the shader
                int lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
                int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
                glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
                glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

                // Draw the mesh
                glBindVertexArray(VAO);

                // Draws the mesh as a series of triangles. The number of indices determines how many vertices are 
                // used from the index buffer, ensuring the entire mesh is rendered correctly.
                glDrawElements(GL_TRIANGLES, mesh->GetIndices().size(), GL_UNSIGNED_INT, 0);
            }
        }
        else
        {

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

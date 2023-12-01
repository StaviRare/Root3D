#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGLAPI.h"
#include "Mesh.h"
#include "MeshGenerator.h"
#include "Debug.h"
#include "Timer.h"
#include "Camera.h"
#include "Entity.h"
#include "MeshData.h"
#include "Renderer.h"

static const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    })";

static const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D textureSampler;
    void main()
    {
        FragColor = texture(textureSampler, TexCoord);
    })";




static GLFWwindow* window = nullptr;
static int modelLoc = -1;
static int viewLoc = -1;
static int projectionLoc = -1;
static unsigned int VAO, VBO, EBO;
static unsigned int fragmentShader;
static unsigned int shaderProgram;
static unsigned int vertexShader;

OpenGLAPI::OpenGLAPI()
{

}

OpenGLAPI::~OpenGLAPI()
{

}

void OpenGLAPI::Initialize()
{
    if (!glfwInit()) {
        //return -1;
    }

    window = glfwCreateWindow(800, 600, "Scene", NULL, NULL);
    if (!window) {
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        glfwTerminate();
    }

    glEnable(GL_DEPTH_TEST);

    // Create and compile the vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create and link the shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Create vertex array and buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set shader uniforms
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");
}

void OpenGLAPI::ClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLAPI::ExecuteRenderCommands()
{
    if (!glfwWindowShouldClose(window))
    {
        if (Camera::exists())
        {
            Camera& camera = Camera::getInstance();

            const auto& bg = camera.backgroundColor;
            glClearColor(bg.r, bg.g, bg.b, bg.a);

            // This code should not be in this location. 
            // Instead, this section should handle the iteration and execution of commands from a list.

            const std::set<Entity*>& entities = EntityPool::GetEntities();

            for (const Entity* entity : entities)
            {
                Renderer* renderer = entity->GetComponent<Renderer>();
                MeshData* meshData = entity->GetComponent<MeshData>();

                if (renderer && meshData)
                {
                    BindTexture(renderer->material.texture);

                    // We're currently using one texture unity.
                    //glActiveTexture(GL_TEXTURE0);


                    Mesh mesh = meshData->mesh;

                    glBufferData(GL_ARRAY_BUFFER, mesh.GetVertices().size() * sizeof(float), mesh.GetVertices().data(), GL_STATIC_DRAW);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndices().size() * sizeof(int), mesh.GetIndices().data(), GL_STATIC_DRAW);

                    // Assuming entity's transform.rotation is stored in degrees
                    Vector3 rotation = entity->transform.eulerAngles;
                    glm::mat4 model = glm::mat4(1.0f);

                    // Convert degrees to radians and apply rotation
                    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the X-axis
                    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the Y-axis
                    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the Z-axis

                    // Apply translation
                    Vector3 position = entity->transform.position;
                    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));

                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                    // Directly translate camera.position to glm::vec3
                    glm::vec3 cameraPosGLM(camera.transform.position.x, camera.transform.position.y, camera.transform.position.z);

                    // Calculate forward vector based on the camera's Y rotation (yaw)
                    glm::vec3 forwardVector = glm::normalize(glm::vec3(glm::sin(camera.transform.eulerAngles.y), 0.0f, -glm::cos(camera.transform.eulerAngles.y)));

                    // Use the translated glm::vec3 for camera position
                    glm::vec3 cameraTarget = cameraPosGLM + forwardVector;

                    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Upward direction

                    // Use the translated glm::vec3 for the lookAt
                    glm::mat4 view = glm::lookAt(cameraPosGLM, cameraTarget, cameraUp);
                    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

                    // Set the projection matrix (perspective projection)
                    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), 800.0f / 600.0f, 0.1f, 100.0f);
                    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

                    // Draw the mesh
                    glBindVertexArray(VAO);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                }
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
    else {

        // destruction. Change logic.
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteProgram(shaderProgram);

        glfwTerminate();
    }
}

void OpenGLAPI::BindTexture(Texture& texture)
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
            std::cerr << "Failed to load texture" << std::endl;
        }
    }
    else
    {
        // Bind existing texture
        glBindTexture(GL_TEXTURE_2D, texture.textureID);
    }
}
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
#include "Camera.h"
#include "RenderQueue.h"

static const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;
    layout(location = 2) in vec3 aNormal;   // Add normal data

    out vec2 TexCoord;
    out vec3 Normal;         // Pass normal data
    out vec3 FragPos;        // Pass fragment position

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        FragPos = vec3(model * vec4(aPos, 1.0)); // Calculate world position of vertex
        Normal = mat3(transpose(inverse(model))) * aNormal; // Calculate normal

        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    })";

static const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    in vec3 Normal;          // Added normal vector
    in vec3 FragPos;         // Fragment position

    out vec4 FragColor;

    uniform sampler2D textureSampler;
    uniform vec3 lightDir;   // Direction of the light
    uniform vec3 lightColor; // Color of the light

    void main()
    {
        // Ambient lighting
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;

        // Diffuse lighting
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, -lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // Combine results
        vec3 result = (ambient + diffuse) * texture(textureSampler, TexCoord).rgb;
        FragColor = vec4(result, 1.0);
    })";




static GLFWwindow* window = nullptr;
static int modelLoc = -1;
static int viewLoc = -1;
static int projectionLoc = -1;
static unsigned int VAO, VBO[3], EBO; // Separate VBOs for positions, texture coordinates, and normals
static unsigned int fragmentShader;
static unsigned int shaderProgram;
static unsigned int vertexShader;
static float aspectRatio = 0;
static float screenWidth = 960;
static float screenHeight = 540;

OpenGLAPI::OpenGLAPI()
{

}

OpenGLAPI::~OpenGLAPI()
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

void OpenGLAPI::Initialize()
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


            while (!RenderQueue::IsEmpty()) {
                RenderCommand* command = RenderQueue::Dequeue();


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

                // Calculate forward vector based on the camera's Y rotation (yaw)
                glm::vec3 forwardVector = glm::normalize(glm::vec3(glm::sin(camera.transform.eulerAngles.y), 0.0f, -glm::cos(camera.transform.eulerAngles.y)));

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
        glDeleteProgram(shaderProgram);

        // Delete shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Terminate GLFW
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
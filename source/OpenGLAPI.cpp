#include "OpenGLAPI.h"
#include <GL/glew.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Mesh.h"
#include "MeshGenerator.h"
#include "Debug.h"
#include "Timer.h"
#include "Camera.h"

const char* vertexShaderSource = R"(
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

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D textureSampler;
    void main()
    {
        FragColor = texture(textureSampler, TexCoord);
    })";




static GLFWwindow* window = nullptr;
int modelLoc = -1;
int viewLoc = -1;
int projectionLoc = -1;
unsigned int VAO, VBO, EBO;
unsigned int texture;
unsigned int fragmentShader;
unsigned int shaderProgram;
unsigned int vertexShader;
double lastFrameTime = 0;

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


    // Create cube mesh
    Mesh mesh = MeshGenerator::GetCube();


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
    glBufferData(GL_ARRAY_BUFFER, mesh.GetVertices().size() * sizeof(float), mesh.GetVertices().data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndices().size() * sizeof(int), mesh.GetIndices().data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load and bind the texture

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Load the texture image (replace "your_texture.png" with the actual image path)
    int width, height, nrChannels;
    unsigned char* data = stbi_load("C:/Users/Stavi/Desktop/Stavi/Profile2_x2BW.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Set texture properties (optional)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set shader uniforms
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");


    lastFrameTime = glfwGetTime();
}

void OpenGLAPI::ClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLAPI::ExecuteRenderCommands()
{

    if (!glfwWindowShouldClose(window)) {
        if (Camera::exists()) {
            
            // Rotate Cube
            float time = glfwGetTime();
            float angle = time * 50.0f;
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            //// Leave cube alone
            //// Set the model matrix (keep the cube stationary at -1 in the z-axis)
            //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
            //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));




            Camera& camera = Camera::getInstance();

            // Directly translate camera.position to glm::vec3
            glm::vec3 cameraPosGLM(camera.position.x, camera.position.y, camera.position.z);

            // Calculate forward vector based on the camera's Y rotation (yaw)
            glm::vec3 forwardVector = glm::normalize(glm::vec3(glm::sin(camera.rotation.y), 0.0f, -glm::cos(camera.rotation.y)));

            // Use the translated glm::vec3 for camera position
            glm::vec3 cameraTarget = cameraPosGLM + forwardVector;

            glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Upward direction

            // Use the translated glm::vec3 for the lookAt
            glm::mat4 view = glm::lookAt(cameraPosGLM, cameraTarget, cameraUp);




            //glm::vec3 forwardVector = glm::normalize(glm::vec3(glm::sin(cameraYaw), 0.0f, -glm::cos(cameraYaw)));


            //// Set the view matrix (position and orient the camera)
            //glm::vec3 cameraTarget = cameraPosition + forwardVector; // Point the camera towards the target
            //glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Upward direction
            //glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
            
            
            
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            // Set the projection matrix (perspective projection)
            glm::mat4 projection = glm::perspective(glm::radians(camera.fov), 800.0f / 600.0f, 0.1f, 100.0f);
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));








            // Bind the texture to the texture unit (e.g., GL_TEXTURE0)
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            // Draw the cube
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        else {

        }
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

#include "ExampleScene.h"
#include "Camera.h"
#include "Time.h"
#include "Input.h"
#include "Mesh.h"
#include "Entity.h"
#include "MeshData.h"
#include "MeshGenerator.h"
#include "Debug.h"
#include "Texture.h"
#include "Renderer.h"
#include "TextureResourceManager.h";
#include "SceneManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static Entity entity;
static Entity entity2;
static Camera camera;
static float cameraMoveSpeed = 3.00f;
static float cameraLookSpeed = 2.00f;

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



static const char* vertexShaderSource2 = R"(
#version 330 core
layout(location = 0) in vec3 aPos; // The position variable has attribute position 0

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); // Transform the vertex position into clip space
})";

static const char* fragmentShaderSource2 = R"(
#version 330 core
out vec4 FragColor;

uniform vec4 ourColor; // We set this variable from the OpenGL code.

void main()
{
    FragColor = ourColor; // Set the fragment color to a uniform value
})";



void initCamera();
void initCube();
void handleCameraMovement();
void handleCubeTransform();



void ExampleScene::init()
{
    initCamera();
    initCube();
}

void ExampleScene::tick()
{
    handleCameraMovement();
    handleCubeTransform();
}


void initCamera()
{
    camera.fov = 60.0f;
    camera.backgroundColor = Color(0.1f, 0, 0.1f, 0);
    camera.transform.position = Vector3(0, 0, 3);
}

void initCube()
{
    Shader shader1(vertexShaderSource, fragmentShaderSource);
    Shader shader2(vertexShaderSource2, fragmentShaderSource2);

    


    // Entity 1
    Material material(shader1);

    material.texture = TextureResourceManager::Load("C:/Users/Stavi/Desktop/Stavi/Profile2_x2BW.png");

    entity.transform.position = Vector3(-1.0, 0, 0);
    //entity.transform.scale *= 1.5;
    Mesh mesh = MeshGenerator::GetCube();
    MeshData* meshData = entity.AddComponent<MeshData>();
    meshData->mesh = mesh;
    Renderer* renderer = entity.AddComponent<Renderer>();
    renderer->material = material;



    // Entity 2
    Material material2(shader2);
    material2.texture = TextureResourceManager::Load("C:/Users/Stavi/Desktop/Stavi/1.png");

    entity2.transform.position = Vector3(1.0f, 0, 0);
    Mesh mesh2 = MeshGenerator::GetCube();
    MeshData* meshData2 = entity2.AddComponent<MeshData>();
    meshData2->mesh = mesh2;
    Renderer* renderer2 = entity2.AddComponent<Renderer>();
    renderer2->material = material2;
}

void handleCameraMovement()
{
    double deltaTime = Time::DeltaTime();

    if (Input::keyDown("d"))
    {
        camera.transform.eulerAngles += Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    if (Input::keyDown("a"))
    {
        camera.transform.eulerAngles -= Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    // Calculate the forward direction based on the Y-axis rotation.
    Vector3 forward(
        -sin(camera.transform.eulerAngles.y),
        0,
        cos(camera.transform.eulerAngles.y)
    );

    // Normalize the forward vector if it isn't normalized already.
    forward = forward.normalized();

    if (Input::keyDown("w"))
    {
        camera.transform.position -= forward * cameraMoveSpeed * deltaTime;
    }

    if (Input::keyDown("s"))
    {
        camera.transform.position += forward * cameraMoveSpeed * deltaTime;
    }
} 

void handleCubeTransform()
{
    entity.transform.eulerAngles += Vector3(1, 1, 0);
}
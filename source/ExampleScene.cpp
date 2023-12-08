#include "ExampleScene.h"
#include "Camera.h"
#include "Timer.h"
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
    // Entity 1
    Material material;
    material.texture = TextureResourceManager::Load("C:/Users/Stavi/Desktop/Stavi/Profile2_x2BW.png");
    material.vertexShader = vertexShaderSource;
    material.fragmentShader = fragmentShaderSource;

    entity.transform.position = Vector3(0, 0, 0);
    Mesh mesh = MeshGenerator::GetCube();
    MeshData* meshData = entity.AddComponent<MeshData>();
    meshData->mesh = mesh;
    Renderer* renderer = entity.AddComponent<Renderer>();
    renderer->material = material;



    // Entity 2
    Material material2;
    material2.texture = TextureResourceManager::Load("C:/Users/Stavi/Desktop/Stavi/1.png");

    entity2.transform.position = Vector3(1.5f, 0, 0);
    Mesh mesh2 = MeshGenerator::GetCube();
    MeshData* meshData2 = entity2.AddComponent<MeshData>();
    meshData2->mesh = mesh2;
    Renderer* renderer2 = entity2.AddComponent<Renderer>();
    renderer2->material = material2;
}

void handleCameraMovement()
{
    double deltaTime = Timer::getDeltaTime();

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
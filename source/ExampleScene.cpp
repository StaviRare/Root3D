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
#include "SceneManager.h"
#include "Resource.h"

static Entity entity;
static Entity entity2;
static Camera camera;
static float cameraMoveSpeed = 3.00f;
static float cameraLookSpeed = 2.00f;


void initCamera();
void initPrimitives();
void handleCameraMovement();
void handleCubeTransform();


void ExampleScene::init()
{
    initCamera();
    initPrimitives();
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

void initPrimitives()
{
    // Entity 1
    Shader shader1 = Resource::LoadShader("shaders/Lit.glsl");
    Material material(shader1);
    material.texture = Resource::LoadTexture("textures/dev.png");

    entity.transform.position = Vector3(-1.0, 0, 0);
    //entity.transform.scale *= 1.5;
    Mesh mesh = MeshGenerator::GetCube();
    MeshData* meshData = entity.AddComponent<MeshData>();
    meshData->mesh = mesh;
    Renderer* renderer = entity.AddComponent<Renderer>();
    renderer->material = material;


    // Entity 2
    Shader shader2 = Resource::LoadShader("shaders/UnlitWobble.glsl");
    Material material2(shader2);
    material2.texture = Resource::LoadTexture("textures/dev_og.png");

    entity2.transform.position = Vector3(1.0f, 0, 0);
    Mesh mesh2 = MeshGenerator::GetCube();
    MeshData* meshData2 = entity2.AddComponent<MeshData>();
    meshData2->mesh = mesh2;
    Renderer* renderer2 = entity2.AddComponent<Renderer>();
    renderer2->material = material2;
}

void handleCameraMovement()
{
    float deltaTime = Time::DeltaTime();

    // Move forward
    if (Input::GetKey("w"))
    {
        camera.transform.position += camera.transform.getForward() * cameraMoveSpeed * deltaTime;
    }

    // Move back
    if (Input::GetKey("s"))
    {
        camera.transform.position -= camera.transform.getForward() * cameraMoveSpeed * deltaTime;
    }

    // Look left
    if (Input::GetKey("d"))
    {
        camera.transform.eulerAngles += Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    // Look right
    if (Input::GetKey("a"))
    {
        camera.transform.eulerAngles -= Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    // Look up
    if (Input::GetKey("e"))
    {
        camera.transform.eulerAngles += Vector3(cameraLookSpeed * deltaTime, 0, 0);
    }

    // Look down
    if (Input::GetKey("q"))
    {
        camera.transform.eulerAngles -= Vector3(cameraLookSpeed * deltaTime, 0, 0);
    }
} 

void handleCubeTransform()
{
    entity.transform.eulerAngles += Vector3(1, 1, 0);
}
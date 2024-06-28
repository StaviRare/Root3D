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
#include "SceneManager.h"
#include "Resource.h"
#include "Light.h"
#include "Graphics.h"
#include "RigidBody.h"
#include "Physics.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "MeshCollider.h"

static Entity entity;
static Entity entity2;
static Entity lightEntity;
static Entity lightEntity2;

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
    camera.transform.position = Vector3(0, 0, 4);

    RigidBody* rigidBody1 = camera.AddComponent<RigidBody>();
    Collider* collider1 = camera.AddComponent<Collider>();
    rigidBody1->IsStatic = true;
}

void initPrimitives()
{
    Shader shader1;
    Shader shader2;
    string graphicsAPI = Graphics::TypeName();

    if (graphicsAPI == "OpenGL")
    {
        shader1 = Resource::LoadShader("shaders/glsl/Lit.glsl");
        shader2 = Resource::LoadShader("shaders/glsl/Unlit.glsl"); // UnlitWobble
    }
    else if (graphicsAPI == "DirectX11")
    {
        shader1 = Resource::LoadShader("shaders/hlsl/Lit.hlsl");
        shader2 = Resource::LoadShader("shaders/hlsl/UnlitTexture.hlsl");
    }
    else if(graphicsAPI == "OpenGLES1")
    {
        Debug::LogWarning("No shaders in OpenGLES1");
    }
    else
    {
        Debug::LogError("Unsupported graphics API. Shaders not set.");
    }


    // static entity 1:
    Material material(shader1);
    material.texture = Resource::LoadTexture("textures/dev.png");
    entity.transform.position = Vector3(0, -1, 0);

    entity.transform.rotation = Quaternion::FromEuler(Vector3(15, 15, 0));

    entity.transform.scale = Vector3(2, 1, 2);
    Mesh mesh = MeshGenerator::GetCube();
    MeshData* meshData = entity.AddComponent<MeshData>();
    Renderer* renderer = entity.AddComponent<Renderer>();
    RigidBody* rigidBody1 = entity.AddComponent<RigidBody>();
    MeshCollider* collider1 = entity.AddComponent<MeshCollider>();

    meshData->mesh = mesh;
    renderer->material = material;
    rigidBody1->IsStatic = true;

    // dynamic entity 2:
    Material material2(shader2);
    material2.texture = Resource::LoadTexture("textures/dev.png");
    entity2.transform.position = Vector3(0.0, 1.0f, 0.0f);
    Mesh mesh2 = MeshGenerator::GetCube();
    MeshData* meshData2 = entity2.AddComponent<MeshData>();
    Renderer* renderer2 = entity2.AddComponent<Renderer>();
    RigidBody* rigidBody2 = entity2.AddComponent<RigidBody>();
    MeshCollider* collider2 = entity2.AddComponent<MeshCollider>();

    meshData2->mesh = mesh2;
    renderer2->material = material2;



    //Physics::SetGravity(-0.5);




    //// Point light
    lightEntity.transform.eulerAngles = Vector3(0, 0, 0);
    lightEntity.transform.position = Vector3(0, 0, 0);
    Light* light = lightEntity.AddComponent<Light>();
    light->type = LightType::Point;
    light->color = Color(0, 1, 0);
    light->range = 1.8f;
    light->intensity = 1;

    // Directional light
    lightEntity2.transform.eulerAngles = Vector3(-1, 0, 0);
    lightEntity2.transform.position = Vector3(0, 0, 0);
    Light* light2 = lightEntity2.AddComponent<Light>();
    light2->type = LightType::Directional;
    light2->color = Color(1, 0, 0);
    light2->intensity = 1;
}

void handleCameraMovement()
{
    float deltaTime = Timer::DeltaTime();

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


    if (Input::GetKey("x"))
    {
        Physics::SetGravity(Vector3(0,-4,0));
    }

    if (Input::GetKey("c"))
    {
        Physics::SetGravity(Vector3(0,4,0));
    }
} 

void handleCubeTransform()
{
    float deltaTime = Timer::DeltaTime();
    float timeSinceInit = Timer::TimeSinceInit();


    //// Rotate
    //entity.transform.eulerAngles += Vector3(-15, -25, 15) * deltaTime;
    //entity.transform.rotation = Quaternion::FromEuler(entity.transform.eulerAngles);


    //// Move
    //float amplitude = -1.0f;
    //entity.transform.position.z += amplitude * Calc::Sin(timeSinceInit) * deltaTime;

}
#include "ExampleScene.h"
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
#include "Camera.h"
#include "CameraController.h"
#include "MiscRotate.h"
#include "TextMesh.h"

static Entity* entity;
static Entity* entity2;
static Entity* lightEntity;
static Entity* lightEntity2;
static Entity* camController;
static Entity* textEntity;

static Shader shaderLit;
static Shader shaderUnlit;


void unInit();
void initCamera();
void initShaders();
void initLighting();
void initStaticCube();
void initDynamicCube();
void initText();

void ExampleScene::onLoad()
{
    initCamera();
    initShaders();
    initLighting();
    initStaticCube();
    initDynamicCube();
    initText();
}

void ExampleScene::onUnload()
{
    unInit();
}

void initCamera()
{
    camController = new Entity();
    camController->AddComponent<Camera>();
    camController->AddComponent<CameraController>();

    // Collide with dynamic box
    camController->AddComponent<BoxCollider>();
    RigidBody* rigidBody1 = camController->AddComponent<RigidBody>();
    rigidBody1->IsStatic = true;
}

void initShaders()
{
    string graphicsAPI = Graphics::TypeName();

    if (graphicsAPI == "OpenGL")
    {
        shaderLit = Resource::LoadShader("shaders/glsl/Lit.glsl");
        shaderUnlit = Resource::LoadShader("shaders/glsl/Unlit.glsl"); // UnlitWobble
    }
    else if (graphicsAPI == "DirectX11")
    {
        shaderLit = Resource::LoadShader("shaders/hlsl/Lit.hlsl");
        shaderUnlit = Resource::LoadShader("shaders/hlsl/UnlitTexture.hlsl");
    }
    else if(graphicsAPI == "OpenGLES1")
    {
        Debug::LogWarning("No shaders in OpenGLES1");
    }
    else
    {
        Debug::LogError("Unsupported graphics API. Shaders not set.");
    }
}

void initLighting()
{
    // Point light
    lightEntity = new Entity();
    lightEntity->transform.eulerAngles = Vector3(0, 0, 0);
    lightEntity->transform.position = Vector3(0, 0, 0);
    Light* light = lightEntity->AddComponent<Light>();
    light->type = LightType::Point;
    light->color = Color(0, 1, 0);
    light->range = 1.8f;
    light->intensity = 1;

    // Directional light
    lightEntity2 = new Entity();
    lightEntity2->transform.eulerAngles = Vector3(-1, 0, 0);
    lightEntity2->transform.position = Vector3(0, 0, 0);
    Light* light2 = lightEntity2->AddComponent<Light>();
    light2->type = LightType::Directional;
    light2->color = Color(1, 0, 0);
    light2->intensity = 1;
}

void initStaticCube()
{
    Material material(shaderLit);
    material.texture = Resource::LoadTexture("textures/dev.png");

    entity = new Entity();
    entity->transform.position = Vector3(0, -1, 0);

    entity->transform.rotation = Quaternion::FromEuler(Vector3(15, 15, 0));

    entity->transform.scale = Vector3(2, 1, 2);
    Mesh mesh = MeshGenerator::GetCube();
    MeshData* meshData = entity->AddComponent<MeshData>();
    Renderer* renderer = entity->AddComponent<Renderer>();
    RigidBody* rigidBody1 = entity->AddComponent<RigidBody>();

    entity->AddComponent<MeshCollider>();
    MiscRotate* miscRotate = entity->AddComponent<MiscRotate>();
    miscRotate->direction = Vector3(-5, -5, 5);

    meshData->mesh = mesh;
    renderer->material = material;
    rigidBody1->IsStatic = true;
}

void initDynamicCube()
{
    Material material2(shaderUnlit);
    material2.texture = Resource::LoadTexture("textures/dev.png");

    entity2 = new Entity();
    entity2->transform.position = Vector3(0.0, 1.0f, 0.0f);
    Mesh mesh2 = MeshGenerator::GetCube();
    MeshData* meshData2 = entity2->AddComponent<MeshData>();
    Renderer* renderer2 = entity2->AddComponent<Renderer>();
    entity2->AddComponent<RigidBody>();
    entity2->AddComponent<MeshCollider>();

    meshData2->mesh = mesh2;
    renderer2->material = material2;
}

void initText()
{
    Font font = Resource::LoadFont("fonts/arial.ttf");
    Mesh textMesh = TextMesh::Generate(font, U"Hello World");

    Material material(shaderUnlit);
    material.texture = Resource::LoadTexture("textures/dev_og.png");

    textEntity = new Entity();
    textEntity->transform.position = Vector3(0, 1, -1);

    MeshData* meshData = textEntity->AddComponent<MeshData>();
    Renderer* renderer = textEntity->AddComponent<Renderer>();

    meshData->mesh = textMesh;
    renderer->material = material;
}

void unInit()
{
    delete entity;
    delete entity2;
    delete lightEntity;
    delete lightEntity2;
    delete camController;
    delete textEntity;

    entity = nullptr;
    entity2 = nullptr;
    lightEntity = nullptr;
    lightEntity2 = nullptr;
    camController = nullptr;
    textEntity = nullptr;
}
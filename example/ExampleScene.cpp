#include "ExampleScene.h"
#include "Timer.h"
#include "Input.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "MeshGenerator.h"
#include "Debug.h"
#include "Texture.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "AssetLoader.h"
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

void ExampleScene::initCamera()
{
    m_playerController = new Entity();
    m_playerController->AddComponent<Camera>();
    m_playerController->AddComponent<CameraController>();
    m_playerController->transform.position = Vector3(0, 0, 4);

    // Collide with dynamic box
    m_playerController->AddComponent<BoxCollider>();
    RigidBody* rigidBody1 = m_playerController->AddComponent<RigidBody>();
    rigidBody1->IsStatic = true;
}

void ExampleScene::initShaders()
{
    string graphicsAPI = Graphics::TypeName();

    if (graphicsAPI == "OpenGL")
    {
        m_shaderLit = AssetLoader::LoadShader("shaders/glsl/Lit.glsl");
        m_shaderUnlit = AssetLoader::LoadShader("shaders/glsl/Unlit.glsl"); // UnlitWobble
    }
    else if (graphicsAPI == "DirectX11")
    {
        m_shaderLit = AssetLoader::LoadShader("shaders/hlsl/Lit.hlsl");
        m_shaderUnlit = AssetLoader::LoadShader("shaders/hlsl/UnlitTexture.hlsl");
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

void ExampleScene::initLighting()
{
    // Point light
    m_pointLight = new Entity();
    m_pointLight->transform.eulerAngles = Vector3(0, 0, 0);
    m_pointLight->transform.position = Vector3(0, 0, 0);
    Light* light = m_pointLight->AddComponent<Light>();
    light->type = LightType::Point;
    light->color = Color(0, 1, 0);
    light->range = 1.8f;
    light->intensity = 1;

    // Directional light
    m_dirLight = new Entity();
    m_dirLight->transform.eulerAngles = Vector3(-1, 0, 0);
    m_dirLight->transform.position = Vector3(0, 0, 0);
    Light* light2 = m_dirLight->AddComponent<Light>();
    light2->type = LightType::Directional;
    light2->color = Color(1, 0, 0);
    light2->intensity = 1;
}

void ExampleScene::initStaticCube()
{
    Material material(*m_shaderLit);
    material.texture = AssetLoader::LoadTexture("textures/dev.png");

    m_staticCube = new Entity();
    m_staticCube->transform.scale = Vector3(2, 1, 2);
    m_staticCube->transform.position = Vector3(0, -1, 0);
    m_staticCube->transform.rotation = Quaternion::FromEuler(Vector3(15, 15, 0));
    Mesh mesh = MeshGenerator::GetCube();
    MeshFilter* meshFilter = m_staticCube->AddComponent<MeshFilter>();
    Renderer* renderer = m_staticCube->AddComponent<Renderer>();
    RigidBody* rigidBody1 = m_staticCube->AddComponent<RigidBody>();
    m_staticCube->AddComponent<MeshCollider>();
    MiscRotate* miscRotate = m_staticCube->AddComponent<MiscRotate>();
    miscRotate->direction = Vector3(-5, -5, 5);

    meshFilter->mesh = mesh;
    renderer->material = material;
    rigidBody1->IsStatic = true;
}

void ExampleScene::initDynamicCube()
{
    Material material2(*m_shaderUnlit);
    material2.texture = AssetLoader::LoadTexture("textures/dev.png");

    m_dynamicCube = new Entity();
    m_dynamicCube->transform.position = Vector3(0.0, 1.0f, 0.0f);
    Mesh mesh2 = MeshGenerator::GetCube();
    MeshFilter* meshFilter2 = m_dynamicCube->AddComponent<MeshFilter>();
    Renderer* renderer2 = m_dynamicCube->AddComponent<Renderer>();
    m_dynamicCube->AddComponent<RigidBody>();
    m_dynamicCube->AddComponent<MeshCollider>();

    meshFilter2->mesh = mesh2;
    renderer2->material = material2;
}

void ExampleScene::initText()
{
    Font font = AssetLoader::LoadFont("fonts/arial.ttf");
    Mesh textMesh = TextMesh::Generate(font, U"Hello World");

    Material material(*m_shaderUnlit);
    material.texture = AssetLoader::LoadTexture("textures/dev_og.png");

    m_textEntity = new Entity();
    m_textEntity->transform.position = Vector3(0, 1, -1);

    MeshFilter* meshFilter = m_textEntity->AddComponent<MeshFilter>();
    Renderer* renderer = m_textEntity->AddComponent<Renderer>();

    meshFilter->mesh = textMesh;
    renderer->material = material;
}

void ExampleScene::unInit()
{
    delete m_staticCube;
    delete m_dynamicCube;
    delete m_pointLight;
    delete m_dirLight;
    delete m_playerController;
    delete m_textEntity;
    delete m_shaderLit;
    delete m_shaderUnlit;

    m_staticCube = nullptr;
    m_dynamicCube = nullptr;
    m_pointLight = nullptr;
    m_dirLight = nullptr;
    m_playerController = nullptr;
    m_textEntity = nullptr;
    m_shaderLit = nullptr;
    m_shaderUnlit = nullptr;
}
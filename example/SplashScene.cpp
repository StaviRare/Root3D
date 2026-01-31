#include "SplashScene.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "MeshGenerator.h"
#include "Debug.h"
#include "Texture.h"
#include "Renderer.h"
#include "AssetLoader.h"
#include "Graphics.h"
#include "Camera.h"
#include "DeferredSceneLoad.h"

void SplashScene::onLoad()
{
    // Camera
    m_camera = new Entity();
    m_camera->transform.position = Vector3(0, 0, 1.5f);
    Camera* camera = m_camera->AddComponent<Camera>();
    camera->backgroundColor = Color(0, 0, 0, 1);

    // Logo
    initShaders();
    m_logo = new Entity();
    m_logo->transform.scale = Vector3(1, 1, 1);
    m_logo->transform.position = Vector3(0, 0, 0);
    m_logo->transform.rotation = Quaternion::FromEuler(Vector3(0, 0, 0));
    Mesh mesh = MeshGenerator::GetQuad();
    MeshFilter* meshFilter = m_logo->AddComponent<MeshFilter>();
    Renderer* renderer = m_logo->AddComponent<Renderer>();
    Material material(*m_shader);
    material.texture = AssetLoader::LoadTexture("textures/logo.png");
    meshFilter->mesh = mesh;
    renderer->material = material;

    // Scene Loader
    m_sceneLoader = new Entity();
    DeferredSceneLoad* sceneLoader = m_sceneLoader->AddComponent<DeferredSceneLoad>();
    sceneLoader->delayDuration = 3;
    sceneLoader->sceneToLoad = 1;
}

void SplashScene::onUnload()
{
    delete m_shader;
    m_shader = nullptr;

    delete m_camera;
    m_camera = nullptr;

    delete m_logo;
    m_logo = nullptr;

    delete m_sceneLoader;
    m_sceneLoader = nullptr;
}

void SplashScene::initShaders()
{
    string graphicsAPI = Graphics::TypeName();

    if (graphicsAPI == "OpenGL")
    {
        m_shader = AssetLoader::LoadShader("shaders/glsl/Unlit.glsl");
    }
    else if (graphicsAPI == "DirectX11")
    {
        m_shader = AssetLoader::LoadShader("shaders/hlsl/UnlitTexture.hlsl");
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